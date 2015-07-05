#pragma once


namespace nui
{
    namespace Base
    {
        template <typename T>
        class NAutoPtr
        {
        public:
            NAutoPtr() : ptr_(0)
            {}

            NAutoPtr(const NAutoPtr& p)
            {
                ptr_ = p.ptr_;
                if(ptr_)
                    ptr_->AddRef();
            }

            NAutoPtr(T* p)
            {
                ptr_ = p;
                if(ptr_)
                    ptr_->AddRef();
            }

            NAutoPtr(int nullPtr)
            {
                UNREFERENCED_PARAMETER(nullPtr);
                NAssertError(nullPtr == 0, _T("only null acceptable"));
                ptr_ = NULL;
            }

            T* Create(LPCSTR filePath, int line)
            {
                nui::Base::NReflect::GetInstance().Create(ptr_, filePath, line);
                NAssertError(ptr_ != NULL, _T("Failed to create obj"));
                return ptr_;
            }

            ~NAutoPtr()
            {
                NSafeRelease(ptr_);
            }

            NAutoPtr& operator = (NAutoPtr& p)
            {
                NSafeRelease(ptr_);
                if(&p != this)
                {
                    ptr_ = p.ptr_;
                    if(ptr_)
                        ptr_->AddRef();
                }
                return (*this);
            }

            NAutoPtr& operator = (int nullPtr)
            {
                UNREFERENCED_PARAMETER(nullPtr);
                NAssertError(nullPtr == 0, _T("only null acceptable"));
                NSafeRelease(ptr_);
                ptr_ = NULL;
                return (*this);
            }

            NAutoPtr& operator = (T* p)
            {
                NSafeRelease(ptr_);
                ptr_ = p;
                if(ptr_)
                    ptr_->AddRef();
                return *this;
            }

            operator T*() const
            {
                return ptr_;
            }

            operator T*&()
            {
                return ptr_;
            }

            T* operator -> () const
            {
                NAssertError(ptr_ != NULL, _T("Null Ptr"));
                return ptr_;
            }

            T* Detach()
            {
                T* p = ptr_;
                ptr_ = NULL;
                return p;
            }

        protected:
            T* ptr_;
        };
    }
}