#include "webengineview.h"

int WebEngineView::m_webPagesCount = 0;

WebEngineView::WebEngineView()
{
	m_myId = m_webPagesCount;
	m_webPagesCount++;
}

int WebEngineView::getWebPageId(){ return m_myId; }
