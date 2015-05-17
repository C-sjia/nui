#pragma once


#include "../NImageDraw.h"


namespace nui
{
    namespace Ui
    {
        class BaseImage : public NImage
        {
        public:
            BaseImage(NResourceLoader* loader);
            ~BaseImage();

            virtual INT Release();
            virtual const Base::NSize& GetSize() const ;

            virtual void NextFrame();
            virtual int GetFrameCount() const;

            virtual bool SetCount(int horzCount, int vertCount);
            virtual void GetCount(int& horzCount, int& vertCount) const;

            void InitForDynamicImage(int frameCount, const Base::NSize& size);
            void InitForStaticImage(int horzCount, int vertCount, const Base::NSize& size);
            virtual void Destroy() = 0;

            bool IsValid() const;

            // NDraw
            virtual void Draw(NRender* render, int horzIndex, int vertIndex, const Base::NRect& rect);

        protected:
            Base::NSize size_;
            int frameIndex_;
            int frameCount_;
            int horzCount_;
            int vertCount_;
            NResourceLoader* loader_;
        };
    }
}
