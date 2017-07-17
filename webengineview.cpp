#include "webengineview.h"

int WebEngineView::m_webPagesCount = 0;

WebEngineView::WebEngineView()
{
	m_myId = m_webPagesCount;
	m_webPagesCount++;
}
/*
static void WebEngineView::resetCount(){ m_webPagesCount = 0; }
static int WebEngineView::getCount(){ return m_webPagesCount; }*/

int WebEngineView::getWebPageId(){ return m_myId; }
