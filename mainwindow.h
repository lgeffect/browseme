#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QWebEngineSettings>
#include <QWebEngineUrlRequestInterceptor>
#include <QNetworkProxy>
#include "webengineview.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	/** БРАУЗЕР ---------------------------------------------------- **/
	int createWebPage(QUrl url);
	void pageProgress(int progress);
	void pageUrlChanged(QUrl url);
	void pageLoaded(bool ok);
	/** БРАУЗЕР ---------------------------------------------------- **/

private slots:
	/** БРАУЗЕР ---------------------------------------------------- **/
	void on_btAddPage_clicked();
	void on_tabsWebPages_currentChanged(int index);
	void on_editPageUrl_returnPressed();
	void on_tabsWebPages_tabCloseRequested(int index);
	void on_btBack_clicked();
	void on_btForward_clicked();
	void on_btReloadStop_clicked();
	void on_treeBrowserHistory_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_allowsChanged_clicked();
	void on_btTurnProxy_clicked();
	void on_cbProxyAuth_clicked();
	/** БРАУЗЕР ---------------------------------------------------- **/


private:
	Ui::MainWindow *ui;
	QNetworkAccessManager* networkManager;
	QNetworkProxy proxy;
};

#endif // MAINWINDOW_H
