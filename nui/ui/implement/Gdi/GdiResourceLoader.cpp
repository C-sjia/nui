#include "stdafx.h"
#include "GdiResourceLoader.h"

#include "../../../Data/NFileSystem.h"
#include "GdiImageDraw.h"
#include "GdiShapeDraw.h"
#include "GdiText.h"
#include "GdiFont.h"
#include "StreamImpl.h"
#include "GdiObjMgr.h"

namespace nui
{
    namespace Ui
    {
        const int DefaultDelayCount = 200;

        GdiResourceLoader::GdiResourceLoader()
        {
        }

        GdiResourceLoader::~GdiResourceLoader()
        {
            ReleaseAllImages();
            GdiObjMgr::Instance().Destroy();
        }

        void GdiResourceLoader::Init()
        {
            GdiObjMgr::Instance().Init();
        }

        NImageDraw* GdiResourceLoader::LoadImage(LPCTSTR filePath)
        {
            Base::NString path(filePath);
            path.MakeLower();
            ImageMap::iterator ite = imageMap_.find(path);
            if(ite != imageMap_.end())
            {
                GdiImageDraw* image = NNew(GdiImageDraw, this);
                ImageData& data = ite->second;
                image->SetBitmaps(path, data.vctBitmaps, data.vctDelayTicks);
                ++ data.refCount;
                return dynamic_cast<NImageDraw*>(image);
            }

            Base::NInstPtr<Data::NFileSystem> fs(MemToolParam);
            nui::Base::NInstPtr<Data::NBuffer> buffer(MemToolParam);
            if(!fs->LoadFile(filePath, buffer))
                return NULL;

            Gdiplus::Bitmap* gdiplusImage = LoadImageFromBuffer(buffer);
            if(gdiplusImage == NULL)
                return NULL;

            Gdiplus::Status status;
            GUID* pDimensionIDs = NULL;
            Gdiplus::PropertyItem* pFrameDelay = NULL;

            ImageData tmpData;
            std::pair<ImageMap::iterator, bool> result = imageMap_.insert(std::make_pair(path, tmpData));
            if(!result.second)
                return NULL;

            ImageData& imageData = result.first->second;
            for(;;)
            {
                imageData.refCount = 1;
                // dimension count
                int dimensionCount = gdiplusImage->GetFrameDimensionsCount();
                if(dimensionCount <= 0)
                    break;

                pDimensionIDs = NNewArray(GUID, dimensionCount);
                status = gdiplusImage->GetFrameDimensionsList(pDimensionIDs, dimensionCount);
                NAssertError(status == Gdiplus::Ok, _T("Failed to GetFrameDimensionsList"));
                if(status != Gdiplus::Ok)
                    break;

                // frame count
                int nFrameCount = gdiplusImage->GetFrameCount(&pDimensionIDs[0]);

                // delay info
                UINT totalSize = gdiplusImage->GetPropertyItemSize(PropertyTagFrameDelay);
                pFrameDelay = (Gdiplus::PropertyItem*)NMalloc(totalSize);
                status = gdiplusImage->GetPropertyItem(PropertyTagFrameDelay, totalSize, pFrameDelay);

                if(status == Gdiplus::Ok)
                {
                    for(int i=0; i<nFrameCount; ++ i)
                    {
                        imageData.vctDelayTicks.AddItem(((long*)pFrameDelay->value)[i]*10);
                    }
                }

                // complete delay count
                if(imageData.vctDelayTicks.Count() != nFrameCount)
                {
                    for(int i=imageData.vctDelayTicks.Count(); i < nFrameCount; ++ i)
                    {
                        imageData.vctDelayTicks.AddItem(DefaultDelayCount);
                    }
                }

                // get all images
                HBITMAP hBitmap = NULL;
                GUID pageGuid = Gdiplus::FrameDimensionTime;
                for(int i=0; i<nFrameCount; ++ i)
                {
                    status = gdiplusImage->SelectActiveFrame(&pageGuid, i);
                    NAssertError(status == Gdiplus::Ok, _T(""));
                    if(status != Gdiplus::Ok)
                        continue;

                    hBitmap = NULL;
                    status = gdiplusImage->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBitmap);
                    NAssertError(status == Gdiplus::Ok, _T(""));
                    if(status != Gdiplus::Ok)
                        continue;

                    imageData.vctBitmaps.AddItem(hBitmap);
                }

                break;
            }

            if(gdiplusImage != NULL)
                delete gdiplusImage;

            if(pDimensionIDs != NULL)
                NDeleteArray(pDimensionIDs);

            if(pFrameDelay != NULL)
                NFree(pFrameDelay);

            GdiImageDraw* image = NNew(GdiImageDraw, this);
            image->SetBitmaps(path, imageData.vctBitmaps, imageData.vctDelayTicks);
            return dynamic_cast<NImageDraw*>(image);
        }

        NShapeDraw* GdiResourceLoader::CreateShape(LPCSTR filePath, int line)
        {
            NShapeDraw* shape = dynamic_cast<NShapeDraw*>(NNewEx(GdiShapeDraw, filePath, line));
            return shape;
        }

        NText* GdiResourceLoader::CreateText(LPCTSTR text, LPCSTR filePath, int line)
        {
            NText* result = dynamic_cast<NText*>(NNewEx(GdiText, filePath, line));
            result->SetText(text);
            return result;
        }

        NFont* GdiResourceLoader::CreateFont(int fontSize, LPCSTR filePath, int line)
        {
            NFont* result = dynamic_cast<NFont*>(NNewEx(GdiFont, filePath, line));
            result->SetFontSize(fontSize);
            return result;
        }

        void GdiResourceLoader::ReleaseImage(const Base::NString& path)
        {
            ImageMap::iterator ite = imageMap_.find(path);
            NAssertError(ite != imageMap_.end(), _T("not found"));
            if(ite == imageMap_.end())
                return;
            ImageData& data = ite->second;
            -- data.refCount;
            if(data.refCount == 0)
            {
                int imgCount = data.vctBitmaps.Count();
                for(int i=0; i<imgCount; ++ i)
                {
                    ::DeleteObject(data.vctBitmaps[i]);
                }
                imageMap_.erase(ite);
            }
        }

        Gdiplus::Bitmap* GdiResourceLoader::LoadImageFromBuffer(Data::NBuffer* buffer)
        {
            LPCVOID pData = buffer->GetBuffer();
            DWORD dwSize = buffer->GetSize();

            Base::NAutoPtr<StreamImpl> stream = NNew(StreamImpl);

            ULONG uWritten = 0;
            HRESULT hResult = stream->Write(pData, dwSize, &uWritten);
            if(FAILED(hResult))
                return NULL;

            hResult = E_FAIL;
            Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(stream);
            if(pBitmap != NULL && pBitmap->GetLastStatus() == Gdiplus::Ok)
            {
                hResult = S_OK;
            }

            if(FAILED(hResult))
            {
                delete pBitmap;
                pBitmap = NULL;
            }

            return pBitmap;
        }

        void GdiResourceLoader::ReleaseAllImages()
        {
            NAssertError(imageMap_.empty(), _T("image leak detected"));

            ImageMap::iterator ite = imageMap_.begin();
            while(ite != imageMap_.end())
            {
                ImageData& data = ite->second;
                -- data.refCount;
                NAssertError(data.refCount == 0, _T("double image leak"));
                if(data.refCount == 0)
                {
                    int imgCount = data.vctBitmaps.Count();
                    for(int i=0; i<imgCount; ++ i)
                    {
                        ::DeleteObject(data.vctBitmaps[i]);
                    }
                    ite = imageMap_.erase(ite);
                }
                else
                {
                    ++ ite;
                }
            }
        }
    }
}
