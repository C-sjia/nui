#pragma once



class TestRichFrameParser : public testing::Test
{
public:
    TestRichFrameParser()
    {
    }
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }
};

TEST_F(TestRichFrameParser, Basic)
{
    // Parse BaseObj
    NInstPtr<NParser> parser(MemToolParam);
    NAutoPtr<NBaseObj> obj = parser->Parse(_T("@TestUI:TestRichFrame"));
    ASSERT_TRUE(obj != NULL);

    // Frame
    NAutoPtr<NRichFrame> richFrame = dynamic_cast<NRichFrame*>((NBaseObj*)obj);
    ASSERT_TRUE(richFrame != NULL);

    EXPECT_EQ(richFrame->GetId(), _T("TestRichFrame"));
    EXPECT_TRUE(richFrame->IsVisible());
    EXPECT_TRUE(richFrame->IsEnabled());

    const NRect& rect = richFrame->GetRect();
    EXPECT_EQ(rect.Left, 123);
    EXPECT_EQ(rect.Top, 34);
    EXPECT_EQ(rect.Width(), 110);
    EXPECT_EQ(rect.Height(), 120);
    EXPECT_TRUE(richFrame->IsAutoSize());

    // Text
    NText* text = richFrame->GetRichText();
    TCHAR szText[100] = {0x6211, 0x53EA, 0x662F, 0x4E00, 0x4E2A, 0x6D4B, 0x8BD5};
    EXPECT_EQ(richFrame->GetText(), szText);
    EXPECT_EQ(text->GetColor(), 0x426594);
    EXPECT_EQ(text->GetBgColor(), 0x726892);
    EXPECT_EQ(text->GetSingleLine(), false);
    EXPECT_EQ(text->GetVertCenter(), true);
    EXPECT_EQ(text->GetHorzCenter(), true);
}
