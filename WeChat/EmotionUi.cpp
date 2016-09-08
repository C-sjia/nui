#include "StdAfx.h"
#include "EmotionUi.h"

LPCTSTR g_EmotionText[] = 
{
    _T("΢Ц"),
    _T("Ʋ��"),
    _T("ɫ"),
    _T("����"),
    _T("����"),
    _T("����"),
    _T("����"),
    _T("����"),
    _T("˯"),
    _T("���"),
    _T("����"),
    _T("��ŭ"),
    _T("��Ƥ"),
    _T("����"),
    _T("����"),
    _T("�ѹ�"),
    _T("��"),
    _T("�亹"),
    _T("ץ��"),
    _T("��"),
    _T("͵Ц"),
    _T("���"),
    _T("����"),
    _T("����"),
    _T("����"),
    _T("��"),
    _T("����"),
    _T("����"),
    _T("��Ц"),
    _T("����"),
    _T("�ܶ�"),
    _T("����"),
    _T("����"),
    _T("��"),
    _T("��"),
    _T("����"),
    _T("˥"),
    _T("����"),
    _T("�ô�"),
    _T("�ټ�"),
    _T("����"),
    _T("�ٱ�"),
    _T("����"),
    _T("�ܴ���"),
    _T("��Ц"),
    _T("��ߺ�"),
    _T("�Һߺ�"),
    _T("��Ƿ"),
    _T("����"),
    _T("ί��"),
    _T("�����"),
    _T("����"),
    _T("����"),
    _T("��"),
    _T("����"),
    _T("�˵�"),
    _T("����"),
    _T("ơ��"),
    _T("����"),
    _T("ƹ��"),
    _T("����"),
    _T("��"),
    _T("��ͷ"),
    _T("õ��"),
    _T("��л"),
    _T("�촽"),
    _T("����"),
    _T("����"),
    _T("����"),
    _T("����"),
    _T("ը��"),
    _T("��"),
    _T("����"),
    _T("ư��"),
    _T("���"),
    _T("����"),
    _T("̫��"),
    _T("����"),
    _T("ӵ��"),
    _T("ǿ"),
    _T("��"),
    _T("����"),
    _T("ʤ��"),
    _T("��ȭ"),
    _T("����"),
    _T("ȭͷ"),
    _T("�"),
    _T("����"),
    _T("NO"),
    _T("OK"),
    _T("����"),
    _T("����"),
    _T("����"),
    _T("����"),
    _T("���"),
    _T("תȦ"),
    _T("��ͷ"),
    _T("��ͷ"),
    _T("����"),
    _T("Ͷ��"),
    _T("����"),
    _T("����"),
    _T("����"),
    _T("��̫��"),
    _T("��̫��")
};

EmotionUi::EmotionUi(void)
{
}

EmotionUi::~EmotionUi(void)
{
}

void EmotionUi::Create(NWindow* parentWindow)
{
    window_.Create(MemToolParam);
    window_->SetStyle(WindowStyle::MenuLike);
    window_->WindowCreatedEvent.AddHandler(this, &EmotionUi::OnWindowCreated);
    window_->Create(parentWindow->GetNative());
    window_->GetRootFrame()->ApplyStyle(_T("@EmotionUi:MainUi"));
    window_->SetMsgFilterCallback(MakeDelegate(this, &EmotionUi::MsgCallback));
}

void EmotionUi::Show(NFrame* frame)
{
    emotionEventData_.emotion = _T("");
    NRect frameRect = frame->GetRootRect();
    NPoint point(frameRect.Left, frameRect.Top);
    ::ClientToScreen(frame->GetWindow()->GetNative(), point);

    NRect wndRect;
    window_->GetRect(wndRect);
    window_->SetPos(point.X, point.Y - wndRect.Height());

    emotionImg_ = window_->GetRootFrame()->GetChildById<NButton*>(_T("emotion"));
    emotionImg_->ClickEvent.AddHandler(MakeDelegate(this, &EmotionUi::OnEmotionClicked));
    emotionRect_ = emotionImg_->GetRootRect();

    window_->SetVisible(true);
}

bool EmotionUi::OnWindowCreated(Base::NBaseObj* source, NEventData* eventData)
{
    return false;
}

bool EmotionUi::OnEmotionClicked(Base::NBaseObj* source, NEventData* eventData)
{
    NFrame::ClickEventData* data = static_cast<NFrame::ClickEventData*>(eventData);

    int horzIndex = 0;
    int vertIndex = 0;

    if(!GetEmotionIndx(data->point.X, data->point.Y, horzIndex, vertIndex))
        return false;

    window_->SetVisible(false);

    emotionEventData_.emotion = g_EmotionText[horzIndex + vertIndex * 15];
    EmotionSelectedEvent.Invoke(NULL, &emotionEventData_);

    return false;
}

bool EmotionUi::MsgCallback(NWindowBase*, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT&)
{
    if(msg != WM_MOUSEMOVE)
        return false;

    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);

    int horzIndex = 0;
    int vertIndex = 0;

    if(!GetEmotionIndx(xPos, yPos, horzIndex, vertIndex))
        return false;

    NString text = g_EmotionText[horzIndex + vertIndex * 15];
    emotionImg_->SetTooltip(text);
    emotionImg_->ShowTooltip();

    return false;
}

void EmotionUi::Destroy()
{
    window_ = NULL;
}

bool EmotionUi::GetEmotionIndx(int x, int y, int& horzIndex, int& vertIndex) const
{
    if(!emotionRect_.Contains(x, y))
        return false;

    x -= emotionRect_.Left;
    y -= emotionRect_.Top;

    horzIndex = x / 29;
    vertIndex = y / 29;

    if(horzIndex < 0 || horzIndex >= 15 || vertIndex < 0 || vertIndex >= 7)
        return false;
    return true;
}