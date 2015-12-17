#include "stdafx.h"

#include "../NFrameBase.h"
#include "../NFrame.h"

#include "../NRenderClip.h"

namespace nui
{
    namespace Ui
    {
        IMPLEMENT_REFLECTION(NFrame);

        NFrame::NFrame()
        {
            bkgDraw_ = NULL;
            Util::Misc::CheckFlag(frameFlags_, NFrame::FlagAutoSize, true);
        }

        NFrame::~NFrame()
        {
            bkgDraw_ = NULL;
            foreDraw_ = NULL;
        }

        void NFrame::Create(NFrame* parentFrame, LPCTSTR frameId, UINT layout, LPCTSTR frameText)
        {
            NAssertError(parentFrame != NULL, _T("Create twice"));
            frameId = frameId == NULL ? _T("") : frameId;
            frameText = frameText == NULL ? _T("") : frameText;
            SetId(frameId);
            SetText(frameText);
            SetLayout(layout);
            if(parentFrame != NULL)
                parentFrame->AddChild(this);
            OnCreate();
        }

        void NFrame::Create(NFrame* parentFrame, LPCTSTR frameId, const Base::NPoint& pos, LPCTSTR frameText)
        {
            SetId(frameId);
            SetText(frameText);
            SetPos(pos.X, pos.Y);
            if(parentFrame != NULL)
                parentFrame->AddChild(this);
            OnCreate();
        }

        void NFrame::Create(NFrame* parentFrame, LPCTSTR frameId, const Base::NRect& rect, LPCTSTR frameText)
        {
            frameId = frameId == NULL ? _T("") : frameId;
            frameText = frameText == NULL ? _T("") : frameText;

            SetAutoSize(false);
            SetId(frameId);
            SetText(frameText);
            SetPos(rect.Left, rect.Top);
            SetSize(rect.Width(), rect.Height());
            if(parentFrame != NULL)
                parentFrame->AddChild(this);
            OnCreate();
        }

        void NFrame::OnCreate()
        {
            ;
        }

        void NFrame::OnParentChanged()
        {
            __super::OnParentChanged();
            if(GetParent() != NULL)
                AutoSize();
        }

        void NFrame::OnClicked(const nui::Base::NPoint& point)
        {
            ClickEventData eventData(point);
            ClickEvent.Invoke(this, &eventData);
            OnMouseUp();
        }

        void NFrame::SetText(const Base::NString& text)
        {
            if(text.IsEmpty() && text_  == NULL)
                return;

            nui::Base::NInstPtr<nui::Data::NStringBundle> stringBundle(MemToolParam);
            nui::Base::NString readText = stringBundle->GetString(text);
            if(text_ == NULL)
            {
                text_ = NUiBus::Instance().GetResourceLoader()->CreateText(readText, MemToolParam);
            }
            else
            {
                if(text_->GetText() == readText)
                    return;
                text_->SetText(readText);
            }
            AutoSize();
            Invalidate();
        }

        Base::NString NFrame::GetText() const
        {
            return text_ == NULL ? Base::NString::EmptyString : text_->GetText();
        }

        NText* NFrame::GetRichText()
        {
            if(text_ == NULL)
            {
                text_ = NUiBus::Instance().GetResourceLoader()->CreateText(_T(""), MemToolParam);
            }
            return text_;
        }

        NCursor* NFrame::GetCursor() const
        {
            return NULL;
        }

        Base::NSize NFrame::GetAutoSize() const
        {
            if(text_ == NULL && foreDraw_ == NULL && bkgDraw_ == NULL)
            {
                Base::NSize autoSize;
                return autoSize;
            }

#undef max

            Base::NSize autoSize;
            if(text_ != NULL && window_ != NULL)
            {
                window_->GetRender()->GetTextSize(text_, font_, autoSize);
            }

            if(foreDraw_ != NULL)
            {
                Base::NSize foreSize = foreDraw_->GetPreferSize();
                autoSize.Width = std::max(autoSize.Width, foreSize.Width);
                autoSize.Height = std::max(autoSize.Height, foreSize.Height);
            }

            if(bkgDraw_ != NULL)
            {
                Base::NSize bkgSize = bkgDraw_->GetPreferSize();
                autoSize.Width = std::max(autoSize.Width, bkgSize.Width);
                autoSize.Height = std::max(autoSize.Height, bkgSize.Height);
            }
            return autoSize;
        }

        NFrame* NFrame::GetChildById(const Base::NString& id, bool recursive)
        {
            return dynamic_cast<NFrame*>(__super::GetChildById(id, recursive));
        }

        NFrame* NFrame::GetChildAtIndex(size_t index)
        {
            if(index >= childs_.size())
                return NULL;
            FrameList::iterator ite = childs_.begin();
            for(size_t i=0; i<index; ++ i)
                ++ ite;
            return dynamic_cast<NFrame*>(*ite);
        }

        NFrame* NFrame::GetChildByPointAndFlag(const Base::NPoint& point, DWORD flags)
        {
            return dynamic_cast<NFrame*>(__super::GetChildByPointAndFlag(point, flags));
        }

        NFrame* NFrame::GetParent() const
        {
            return dynamic_cast<NFrame*>(__super::GetParent());
        }

        void NFrame::SetBkgDraw(NDraw* bkgDraw)
        {
            if(bkgDraw == bkgDraw_)
                return;
            bkgDraw_ = bkgDraw;
            Invalidate();
        }

        NDraw* NFrame::GetBkgDraw() const
        {
            return bkgDraw_;
        }

        void NFrame::SetForeDraw(NDraw* foreDraw)
        {
            if(foreDraw == foreDraw_)
                return;
            foreDraw_ = foreDraw;
            Invalidate();
        }

        NDraw* NFrame::GetForeDraw() const
        {
            return foreDraw_;
        }

        // Draw
        void NFrame::DrawBkg(NRender* render, const Base::NRect& rect) const
        {
            if(bkgDraw_ == NULL)
                return;
            int horzIndex, vertIndex;
            GetDrawIndex(horzIndex, vertIndex);
            bkgDraw_->Draw(render, horzIndex, vertIndex, rect);
        }

        void NFrame::DrawFore(NRender* render, const Base::NRect& rect) const
        {
            if(foreDraw_ == NULL)
                return;
            int horzIndex, vertIndex;
            GetDrawIndex(horzIndex, vertIndex);
            foreDraw_->Draw(render, horzIndex, vertIndex, rect);
        }

        void NFrame::DrawContent(NRender* render, const Base::NRect& rect) const
        {
            if(text_ != NULL)
                render->DrawText(text_, font_, rect);
        }

    }
}
