#ifndef WEBENGINEVIEW_H
#define WEBENGINEVIEW_H
#include <QWebEngineView>

class WebEngineView : public QWebEngineView
{
public:
	WebEngineView();
	int getWebPageId();

	static void resetCount(){ m_webPagesCount = 0; }
	static int getCount() { return m_webPagesCount; }
private:
	int m_myId;
	static int m_webPagesCount;
};


#endif // WEBENGINEVIEW_H
