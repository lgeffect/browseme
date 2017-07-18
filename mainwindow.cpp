#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	createWebPage(QUrl(""));
}

MainWindow::~MainWindow()
{
	delete ui;
}

/** БРАУЗЕР ---------------------------------------------------- **/
void MainWindow::createWebPage(QUrl url){
	// Создаем новую вкладку в браузере
	WebEngineView* newPage = new WebEngineView();
	newPage->setUrl(url);
	newPage->resize(ui->tabsWebPages->width(), ui->tabsWebPages->height());
	ui->tabsWebPages->addTab(newPage, "");
	newPage->show();

	// Коннектим линию загрузки с загрузкой страницы и смену url-а
	connect(newPage, &WebEngineView::loadProgress, this, &MainWindow::pageProgress);
	connect(newPage, &WebEngineView::urlChanged, this,  &MainWindow::pageUrlChanged);
	connect(newPage, &WebEngineView::loadFinished, this, &MainWindow::pageLoaded);
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
	createWebPage(QUrl(""));
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
	if(!url.startsWith("http")) url = "http://" + url;
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
	if(column == 0 && (item->text(0).startsWith("http") || item->text(0).startsWith("ftp"))){
		createWebPage(QUrl(item->text(0)));
	}
}
/** БРАУЗЕР ---------------------------------------------------- **/

