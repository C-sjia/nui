#pragma once


#include "../BaseImageDraw.h"


namespace nui
{
    namespace Ui
    {
        class GdiImage : public BaseImage
        {
        public:
            GdiImage(NResourceLoader* loader);
            ~GdiImage();

            virtual int NextDelayValue(int index);

            virtual void Destroy();

            void SetBitmaps(const Base::NString& imagePath, const Data::NArrayT<HBITMAP>& vctBitmaps, const Data::NArrayT<int>& vctDelayCount);
            HBITMAP GetHBitmap(int index) const;

        private:
            Base::NString imagePath_;
            const Data::NArrayT<HBITMAP>* vctBitmaps_;
            const Data::NArrayT<int>* vctDelayCount_;
        };
    }
}
