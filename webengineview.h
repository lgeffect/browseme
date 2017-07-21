#ifndef WEBENGINEVIEW_H
#define WEBENGINEVIEW_H
#include <QWebEngineView>
#include <QWebEngineProfile>

class WebEngineView : public QWebEngineView
{
public:
	WebEngineView();
	int getWebPageId();

private:
	int m_myId;
	static int m_webPagesCount;
};


#endif // WEBENGINEVIEW_H
