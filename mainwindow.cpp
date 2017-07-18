#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	networkManager = new QNetworkAccessManager();
	createWebPage(QUrl(""));

	// Соединяем сигнал нажатия на галочках с слотом изменения настроек
	connect(ui->cbAllowAutoImg, SIGNAL(clicked(bool)), this, SLOT(on_allowsChanged_clicked()));
	connect(ui->cbAllowGeoloc, SIGNAL(clicked(bool)), this, SLOT(on_allowsChanged_clicked()));
	connect(ui->cbAllowInsecContent, SIGNAL(clicked(bool)), this, SLOT(on_allowsChanged_clicked()));
	connect(ui->cbAllowJavaScripts, SIGNAL(clicked(bool)), this, SLOT(on_allowsChanged_clicked()));
	connect(ui->cbAllowJSClipboard, SIGNAL(clicked(bool)), this, SLOT(on_allowsChanged_clicked()));
	connect(ui->cbAllowJSOpen, SIGNAL(clicked(bool)), this, SLOT(on_allowsChanged_clicked()));
	connect(ui->cbAllowPlugins, SIGNAL(clicked(bool)), this, SLOT(on_allowsChanged_clicked()));
	connect(ui->cbAllowSound, SIGNAL(clicked(bool)), this, SLOT(on_allowsChanged_clicked()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

/** БРАУЗЕР ---------------------------------------------------- **/
int MainWindow::createWebPage(QUrl url){
	// Создаем новую вкладку в браузере
	WebEngineView* newPage = new WebEngineView();
	newPage->setUrl(url);
	newPage->resize(ui->tabsWebPages->width(), ui->tabsWebPages->height());
	//newPage->settings()->setAttribute(QWebEngineSettings::AutoLoadIconsForPage, true);
	newPage->settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, true);
	int id = ui->tabsWebPages->addTab(newPage, "");
	newPage->show();

	// Устанавливаем наш обработчик запросов

	// Обновляем настройки страниц
	on_allowsChanged_clicked();

	// Коннектим линию загрузки с загрузкой страницы и смену url-а
	connect(newPage, &WebEngineView::loadProgress, this, &MainWindow::pageProgress);
	connect(newPage, &WebEngineView::urlChanged, this,  &MainWindow::pageUrlChanged);
	connect(newPage, &WebEngineView::loadFinished, this, &MainWindow::pageLoaded);

	return id;
}

void MainWindow::pageProgress(int progress){
	// Получаем текущую страницу и ту которая послала сигнал
	WebEngineView* page = (WebEngineView*)sender();
	WebEngineView* currPage = (WebEngineView*)ui->tabsWebPages->currentWidget();

	// Если совпадают, показываем прогресс загрузки
	if(page->getWebPageId() == currPage->getWebPageId()){
		// Изменяем надпись кнопки
		if(progress == 100){
			progress = 0;
			ui->btReloadStop->setText("O");
		}else{
			ui->btReloadStop->setText("X");
		}
		// Устанавливаем текущее значение загрузки страницы
		ui->barLoadProgress->setValue(progress);
	}
}

void MainWindow::pageUrlChanged(QUrl url){
	// Получаем текущую страницу и ту которая послала сигнал
	WebEngineView* page = (WebEngineView*)sender();
	WebEngineView* currPage = (WebEngineView*)ui->tabsWebPages->currentWidget();

	// Если совпадают, то при изменении url меняем его в адресной строке
	if(page->getWebPageId() == currPage->getWebPageId()){
		ui->editPageUrl->setText(url.toString());
		ui->editPageUrl->setCursorPosition(0);
	}
}

void MainWindow::pageLoaded(bool ok){
	// Если загрузка страницы завершена, то устанавливаем на нужной вкладке имя
	if(ok){
		for(int i = 0;i<ui->tabsWebPages->count();i++){
			WebEngineView* page = (WebEngineView*)ui->tabsWebPages->widget(i);
			WebEngineView* pageSender = (WebEngineView*)sender();
			if(page->getWebPageId() == pageSender->getWebPageId()){
				ui->tabsWebPages->setTabText(i, pageSender->title().size() > 15 ? pageSender->title().mid(0, 12)+QString("...") : pageSender->title());
				// Не работает о_О ui->tabsWebPages->setTabIcon(i, pageSender->icon());
				if(ui->tabsWebPages->tabText(i) == "about:blank"){
					ui->tabsWebPages->setTabText(i, "Новая вкладка");
				}else{
					// Добавляем адрес в историю вместе с датой
					QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeBrowserHistory);

					QDateTime datetime = QDateTime::currentDateTime();
					item->setText(0, pageSender->title());
					item->setText(1, datetime.toString());

					ui->treeBrowserHistory->addTopLevelItem(item);
					QTreeWidgetItem* subitem = new QTreeWidgetItem(item);
					subitem->setText(0,pageSender->url().toString());
				}
			}
		}
	}
}

void MainWindow::on_btAddPage_clicked()
{
	ui->tabsWebPages->setCurrentIndex(createWebPage(QUrl("")));
}

void MainWindow::on_tabsWebPages_currentChanged(int index)
{
	// При смене вкладок изменяем и все управляющие элементы
	ui->barLoadProgress->setValue(0);
	ui->btReloadStop->setText("O");
	WebEngineView* currPage = (WebEngineView*)ui->tabsWebPages->widget(index);
	ui->editPageUrl->setText(currPage->url().toString());
}

void MainWindow::on_editPageUrl_returnPressed()
{
	// Приводим наш url к нормальному виду и переходим по нему
	QString url = ui->editPageUrl->text();
	if(url.startsWith("http:\\\\") || url.startsWith("ftp:\\\\")) url.replace(":\\\\", "://");
	if(!url.startsWith("http") && !url.startsWith("ftp") && !url.startsWith("file")) url = "http://" + url;
	WebEngineView* currPage = (WebEngineView*)ui->tabsWebPages->currentWidget();
	currPage->load(QUrl(url));
}

void MainWindow::on_tabsWebPages_tabCloseRequested(int index)
{
	// Закрываем текущую вкладку и дисконнектим все сигналы
	WebEngineView* closedPage = (WebEngineView*)ui->tabsWebPages->widget(index);
	if(ui->tabsWebPages->count() == 1){
		closedPage->setUrl(QUrl(""));
	}else{
		ui->tabsWebPages->removeTab(index);
		disconnect(closedPage, &WebEngineView::loadProgress, this, &MainWindow::pageProgress);
		disconnect(closedPage, &WebEngineView::urlChanged, this,  &MainWindow::pageUrlChanged);
		disconnect(closedPage, &WebEngineView::loadFinished, this, &MainWindow::pageLoaded);
		closedPage->deleteLater();
	}

}

void MainWindow::on_btBack_clicked()
{
	WebEngineView* currPage = (WebEngineView*)ui->tabsWebPages->currentWidget();
	currPage->back();
}

void MainWindow::on_btForward_clicked()
{
	WebEngineView* currPage = (WebEngineView*)ui->tabsWebPages->currentWidget();
	currPage->forward();
}

void MainWindow::on_btReloadStop_clicked()
{
	// Если страница загружается, останавливаем, иначе перезагружаем
	WebEngineView* currPage = (WebEngineView*)ui->tabsWebPages->currentWidget();
	if(ui->btReloadStop->text() == "O"){
		currPage->reload();
	}else{
		currPage->stop();
	}
}

void MainWindow::on_treeBrowserHistory_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	// Переходим на новую вкладку и на страницу из истории
	if(column == 0 && (item->text(0).startsWith("http") || item->text(0).startsWith("ftp") || item->text(0).startsWith("file"))){
		ui->tabsBrowser->setCurrentIndex(0);
		ui->tabsWebPages->setCurrentIndex(createWebPage(QUrl(item->text(0))));
	}
}

void MainWindow::on_allowsChanged_clicked()
{
	// При изменении разрешений, обновляем настройки страниц
	for(int i = 0;i<ui->tabsWebPages->count();i++){
		WebEngineView* page = (WebEngineView*)ui->tabsWebPages->widget(i);

		if(ui->cbAllowAutoImg->isChecked())
				page->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, true);
		else	page->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);

		if(ui->cbAllowJavaScripts->isChecked())
				page->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
		else	page->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, false);


		if(ui->cbAllowPlugins->isChecked())
				page->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
		else	page->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, false);


		if(ui->cbAllowGeoloc->isChecked())
				page->settings()->setAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins, true);
		else	page->settings()->setAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins, false);


		if(ui->cbAllowInsecContent->isChecked())
				page->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
		else	page->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);


		if(ui->cbAllowJSClipboard->isChecked())
				page->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
		else	page->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, false);


		if(ui->cbAllowJSOpen->isChecked())
				page->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
		else	page->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);

		if(ui->cbAllowSound->isChecked())
				page->page()->setAudioMuted(false);
		else	page->page()->setAudioMuted(true);

	}
}

/** БРАУЗЕР ---------------------------------------------------- **/

