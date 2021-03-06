/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#include "fullfunctionwidget.h"
#include "ui_fullfunctionwidget.h"
#include <QDesktopWidget>
#include <QHeaderView>
#include "src/Style/style.h"
#include <QDebug>
#include <QSvgRenderer>
#include <QPainter>

FullFunctionWidget::FullFunctionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FullFunctionWidget)
{
    ui->setupUi(this);

    initWidget();
}

FullFunctionWidget::~FullFunctionWidget()
{
    delete ui;
    delete pUkuiMenuInterface;
}

void FullFunctionWidget::initWidget()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setStyleSheet("border:0px;background:transparent;");
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    applistWid=new QWidget(this);
    iconlistWid=new QWidget(this);
    this->setFixedSize(Style::MainViewWidWidth,
                       Style::AppListWidHeight);
    applistWid->setFixedSize(Style::AppListWidWidth,this->height());
    iconlistWid->setFixedSize(Style::LeftWidWidth,this->height());

    mainLayout=new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    applistWid->setStyleSheet("border:0px;background:transparent;");
    iconlistWid->setStyleSheet("border:0px;background:transparent");
    mainLayout->addWidget(iconlistWid);
    mainLayout->addWidget(applistWid);
    this->setLayout(mainLayout);

    pUkuiMenuInterface=new UkuiMenuInterface;

    iconlist.clear();
    QString recentstr=":/data/img/mainviewwidget/recent-gray.svg";
    QString netstr=":/data/img/mainviewwidget/net-gray.svg";
    QString socialstr=":/data/img/mainviewwidget/social-gray.svg";
    QString avstr=":/data/img/mainviewwidget/video-gray.svg";
    QString developstr=":/data/img/mainviewwidget/develop-gray.svg";
    QString imgstr=":/data/img/mainviewwidget/img-gray.svg";
    QString gamestr=":/data/img/mainviewwidget/game-gray.svg";
    QString officestr=":/data/img/mainviewwidget/office-gray.svg";
    QString readingstr=":/data/img/mainviewwidget/reading-gray.svg";
    QString systemstr=":/data/img/mainviewwidget/system-gray.svg";
    QString otherstr=":/data/img/mainviewwidget/other-gray.svg";
    iconlist.append(recentstr);
    iconlist.append(netstr);
    iconlist.append(socialstr);
    iconlist.append(avstr);
    iconlist.append(developstr);
    iconlist.append(imgstr);
    iconlist.append(gamestr);
    iconlist.append(officestr);
    iconlist.append(readingstr);
    iconlist.append(systemstr);
    iconlist.append(otherstr);

    iconlightlist.clear();
    QString recentlightstr=":/data/img/mainviewwidget/recent.svg";
    QString netlightstr=":/data/img/mainviewwidget/net.svg";
    QString sociallightstr=":/data/img/mainviewwidget/social.svg";
    QString avlightstr=":/data/img/mainviewwidget/video.svg";
    QString developlightstr=":/data/img/mainviewwidget/develop.svg";
    QString imglightstr=":/data/img/mainviewwidget/img.svg";
    QString gamelightstr=":/data/img/mainviewwidget/game.svg";
    QString officelightstr=":/data/img/mainviewwidget/office.svg";
    QString readinglightstr=":/data/img/mainviewwidget/reading.svg";
    QString systemlightstr=":/data/img/mainviewwidget/system.svg";
    QString otherlightstr=":/data/img/mainviewwidget/other.svg";
    iconlightlist.append(recentlightstr);
    iconlightlist.append(netlightstr);
    iconlightlist.append(sociallightstr);
    iconlightlist.append(avlightstr);
    iconlightlist.append(developlightstr);
    iconlightlist.append(imglightstr);
    iconlightlist.append(gamelightstr);
    iconlightlist.append(officelightstr);
    iconlightlist.append(readinglightstr);
    iconlightlist.append(systemlightstr);
    iconlightlist.append(otherlightstr);

    functionnamelist.clear();
    functionnamelist.append(tr("Recently"));
    functionnamelist.append(tr("Internet"));
    functionnamelist.append(tr("Social"));
    functionnamelist.append(tr("Video"));
    functionnamelist.append(tr("Development"));
    functionnamelist.append(tr("Image"));
    functionnamelist.append(tr("Game"));
    functionnamelist.append(tr("Office"));
    functionnamelist.append(tr("Education"));
    functionnamelist.append(tr("System"));
    functionnamelist.append(tr("Others"));

    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeOutSlot()));

    initAppListWidget();
    initIconListWidget();
}

/**
 * 初始化应用列表界面
 */
void FullFunctionWidget::initAppListWidget()
{
    QHBoxLayout* layout=new QHBoxLayout(applistWid);
    layout->setContentsMargins(0,0,0,0);
    applistWid->setLayout(layout);

    scrollarea=new ScrollArea;
    scrollareawid=new QWidget;
    scrollarea->setFixedSize(applistWid->width(),applistWid->height());
    scrollarea->setWidget(scrollareawid);
    scrollarea->setWidgetResizable(true);
    scrollareawidLayout=new QVBoxLayout;
    scrollareawidLayout->setContentsMargins(0,0,0,0);
    scrollareawidLayout->setSpacing(0);
    scrollareawid->setLayout(scrollareawidLayout);
    layout->addWidget(scrollarea);
    connect(scrollarea->verticalScrollBar(),SIGNAL(valueChanged(int)),
            this,SLOT(valueChangedSlot(int)));
    fillAppList();
}

/**
 * 填充应用列表
 */
void FullFunctionWidget::fillAppList()
{
    classificationbtnlist.clear();
    classificationbtnrowlist.clear();

    QVector<QStringList> vector=pUkuiMenuInterface->getFunctionalClassification();
    QStringList recentlist=vector.at(0);
    if(!recentlist.isEmpty())
    {
        insertClassificationBtn(tr("Recently"));
        insertAppList(recentlist,0);
    }

    QStringList netlist=vector.at(1);
    if(!netlist.isEmpty())
    {
        insertClassificationBtn(tr("Internet"));
        insertAppList(netlist,0);
    }
    QStringList sociallist=vector.at(2);
    if(!sociallist.isEmpty())
    {
        insertClassificationBtn(tr("Social"));
        insertAppList(sociallist,0);
    }
    QStringList avlist=vector.at(3);
    if(!avlist.isEmpty())
    {
        insertClassificationBtn(tr("Video"));
        insertAppList(avlist,0);
    }
    QStringList developlist=vector.at(4);
    if(!developlist.isEmpty())
    {
        insertClassificationBtn(tr("Development"));
        insertAppList(developlist,0);
    }
    QStringList graphicslist=vector.at(5);
    if(!graphicslist.isEmpty())
    {
        insertClassificationBtn(tr("Image"));
        insertAppList(graphicslist,0);
    }
    QStringList gamelist=vector.at(6);
    if(!gamelist.isEmpty())
    {
        insertClassificationBtn(tr("Game"));
        insertAppList(gamelist,0);
    }
    QStringList officelist=vector.at(7);
    if(!officelist.isEmpty())
    {
        insertClassificationBtn(tr("Office"));
        insertAppList(officelist,0);
    }
    QStringList educationlist=vector.at(8);
    if(!educationlist.isEmpty())
    {
        insertClassificationBtn(tr("Education"));
        insertAppList(educationlist,0);
    }

    QStringList systemadminlist=vector.at(9);
    if(!systemadminlist.isEmpty())
    {
        insertClassificationBtn(tr("System"));
        insertAppList(systemadminlist,0);
    }
    QStringList otherlist=vector.at(10);
    if(!otherlist.isEmpty())
    {
        insertClassificationBtn(tr("Others"));
        insertAppList(otherlist,0);
    }

    resizeScrollAreaControls();
}

void FullFunctionWidget::insertClassificationBtn(QString btnname)
{
    PushButton* classificationbtn=new PushButton(btnname,scrollarea->width()-12,20);
    classificationbtn->setFixedSize(scrollarea->width()-12,20);
    scrollareawidLayout->addWidget(classificationbtn);
    classificationbtnlist.append(btnname);

}

void FullFunctionWidget::insertAppList(QStringList appnamelist, int type)
{
    FullListView* listview=new FullListView(this,2);
    scrollareawidLayout->addWidget(listview);
    data.clear();
    for(int i=0;i<appnamelist.count();i++)
    {

        QString desktopfp=pUkuiMenuInterface->getDesktopPathByAppName(appnamelist.at(i));
        data.append(desktopfp);
    }

    if(type==0)
        listview->addData(data);
    else
        listview->updateData(data);
    connect(listview,SIGNAL(sendItemClickedSignal(QString)),this,SLOT(execApplication(QString)));
    connect(listview,SIGNAL(sendFixedOrUnfixedSignal()),this,SIGNAL(sendUpdateAppListSignal()));
    connect(listview,SIGNAL(sendHideMainWindowSignal()),this,SIGNAL(sendHideMainWindowSignal()));
}

/**
 * 执行应用程序
 */
void FullFunctionWidget::execApplication(QString appname)
{
    Q_EMIT sendHideMainWindowSignal();
    QString execpath=pUkuiMenuInterface->getAppExec(pUkuiMenuInterface->getDesktopPathByAppName(appname));
    //移除启动参数%u或者%U
    if(execpath.contains("%"))
    {
        int index=execpath.indexOf(QString("%").at(0));
        execpath.remove(index-1,3);
    }
    QProcess::startDetached(execpath);
}

/**
 * 更新应用列表
 */
void FullFunctionWidget::updateAppListView()
{
    //刷新应用列表界面
    for(int index=scrollareawidLayout->count()-1;index>=0;index--)
    {
            QLayoutItem* item=scrollareawidLayout->takeAt(index);
            QWidget* wid=item->widget();
            scrollareawidLayout->removeWidget(wid);
            wid->setParent(nullptr);
            delete wid;
    }

    classificationbtnlist.clear();
    classificationbtnrowlist.clear();

    QVector<QStringList> vector=pUkuiMenuInterface->getFunctionalClassification();
    QStringList recentlist=vector.at(0);
    if(!recentlist.isEmpty())
    {
        insertClassificationBtn(tr("Recently"));
        insertAppList(recentlist,1);
    }

    QStringList netlist=vector.at(1);
    if(!netlist.isEmpty())
    {
        insertClassificationBtn(tr("Internet"));
        insertAppList(netlist,1);
    }
    QStringList sociallist=vector.at(2);
    if(!sociallist.isEmpty())
    {
        insertClassificationBtn(tr("Social"));
        insertAppList(sociallist,1);
    }
    QStringList avlist=vector.at(3);
    if(!avlist.isEmpty())
    {
        insertClassificationBtn(tr("Video"));
        insertAppList(avlist,1);
    }
    QStringList developlist=vector.at(4);
    if(!developlist.isEmpty())
    {
        insertClassificationBtn(tr("Development"));
        insertAppList(developlist,1);
    }
    QStringList graphicslist=vector.at(5);
    if(!graphicslist.isEmpty())
    {
        insertClassificationBtn(tr("Image"));
        insertAppList(graphicslist,1);
    }
    QStringList gamelist=vector.at(6);
    if(!gamelist.isEmpty())
    {
        insertClassificationBtn(tr("Game"));
        insertAppList(gamelist,1);
    }
    QStringList officelist=vector.at(7);
    if(!officelist.isEmpty())
    {
        insertClassificationBtn(tr("Office"));
        insertAppList(officelist,1);
    }
    QStringList educationlist=vector.at(8);
    if(!educationlist.isEmpty())
    {
        insertClassificationBtn(tr("Education"));
        insertAppList(educationlist,1);
    }

    QStringList systemadminlist=vector.at(9);
    if(!systemadminlist.isEmpty())
    {
        insertClassificationBtn(tr("System"));
        insertAppList(systemadminlist,1);
    }
    QStringList otherlist=vector.at(10);
    if(!otherlist.isEmpty())
    {
        insertClassificationBtn(tr("Others"));
        insertAppList(otherlist,1);
    }

    resizeScrollAreaControls();

    //刷新图标列表界面
    Q_FOREACH (QAbstractButton* button, buttonList){
        pBtnGroup->removeButton(button);
    }
    buttonList.clear();
    for(int index=iconlistscrollareawidLayout->count()-2;index>0;index--)
    {
        QLayoutItem* item=iconlistscrollareawidLayout->takeAt(index);
        QWidget* wid=item->widget();
        iconlistscrollareawidLayout->removeWidget(wid);
        wid->setParent(nullptr);
        delete wid;
    }
    for(int i=0;i<classificationbtnlist.size();i++)
    {
        FunctionClassifyButton* iconbtn=new FunctionClassifyButton(this,
                                                                   Style::LeftBtnWidth,
                                                                   Style::LeftBtnHeight,
                                                                   Style::LeftIconSize,
                                                                   Style::LeftFontSize,
                                                                   iconlist.at(functionnamelist.indexOf(classificationbtnlist.at(i))),
                                                                   iconlightlist.at(functionnamelist.indexOf(classificationbtnlist.at(i))),
                                                                   ClassifyBtnHoverBackground,
                                                                   ClassifyBtnHoverBackground,
                                                                   2,
                                                                   classificationbtnlist.at(i),
                                                                   true,
                                                                   true);
        buttonList.append(iconbtn);
        iconlistscrollareawidLayout->insertWidget(i+1,iconbtn);
        connect(iconbtn,SIGNAL(buttonClicked(QAbstractButton*)),pBtnGroup, SIGNAL(buttonClicked(QAbstractButton*)));
    }

    int id=0;
    Q_FOREACH (QAbstractButton* btn, buttonList) {
        pBtnGroup->addButton(btn,id++);
    }
    iconlistscrollarea->widget()->adjustSize();
    pBtnGroup->button(0)->click();
}

/**
 * 设置scrollarea所填充控件大小
 */
void FullFunctionWidget::resizeScrollAreaControls()
{
    int pos=0;
    classificationbtnrowlist.append(QString::number(pos));
    int row=0;
    while(row<scrollareawidLayout->count()/2)
    {
        //分类按钮
//        QLayoutItem* classificationbtnwidItem=scrollareawidLayout->itemAt(row*2);
//        QWidget* classificationbtnwid=classificationbtnwidItem->widget();
//        QPushButton* classificationbtn=qobject_cast<QPushButton*>(classificationbtnwid);
//        classificationbtn->setFixedSize(scrollarea->width()-32,20);
        //应用界面
        QLayoutItem* widItem=scrollareawidLayout->itemAt(row*2+1);
        QWidget* wid=widItem->widget();
        FullListView* listview=qobject_cast<FullListView*>(wid);
        listview->adjustSize();
        int dividend=(scrollarea->width()-Style::SliderSize)/Style::AppListGridSizeWidth;
        int rowcount=0;
        if(listview->model()->rowCount()%dividend>0)
        {
            rowcount=listview->model()->rowCount()/dividend+1;
        }
        else
        {
            rowcount=listview->model()->rowCount()/dividend;

        }

        listview->setFixedSize(scrollarea->width()-Style::SliderSize+1,listview->gridSize().height()*rowcount);
        if(row<scrollareawidLayout->count()/2-1)
        {
            pos+=(20+listview->height());
            classificationbtnrowlist.append(QString::number(pos));
        }
        row++;
    }
    scrollarea->widget()->adjustSize();
}

/**
 * 初始化图标列表界面
 */
void FullFunctionWidget::initIconListWidget()
{
    iconlistLayout=new QHBoxLayout(iconlistWid);
    iconlistLayout->setContentsMargins(Style::LeftMargin,0,Style::RightMargin,0);
    iconlistLayout->setSpacing(0);
    iconlistWid->setLayout(iconlistLayout);

    iconlistscrollarea=new ClassifyScrollArea();
    iconlistscrollarea->setFixedSize(Style::LeftBtnWidth,iconlistWid->height());
    iconlistscrollareaWid=new QWidget;
    iconlistscrollareawidLayout=new QVBoxLayout;
    iconlistscrollareawidLayout->setContentsMargins(0,0,0,0);
    iconlistscrollareawidLayout->setSpacing(Style::LeftSpaceBetweenItem);
    iconlistscrollareaWid->setLayout(iconlistscrollareawidLayout);
    iconlistscrollarea->setWidget(iconlistscrollareaWid);
    iconlistLayout->addWidget(iconlistscrollarea);
    pBtnGroup=new QButtonGroup(iconlistscrollareaWid);
    initIconListScrollArea();
}

/**
 * 初始化图标列表界面数据表格iconlisttableWid
 */
void FullFunctionWidget::initIconListScrollArea()
{
    pIconListTopSpacer=new QSpacerItem(40,20,QSizePolicy::Fixed,QSizePolicy::Expanding);
    pIconListBottomSpacer=new QSpacerItem(40,20,QSizePolicy::Fixed,QSizePolicy::Expanding);

    iconlistscrollareawidLayout->addItem(pIconListTopSpacer);
    for(int i=0;i<classificationbtnlist.size();i++)
    {
        FunctionClassifyButton* iconbtn=new FunctionClassifyButton(this,
                                                                   Style::LeftBtnWidth,
                                                                   Style::LeftBtnHeight,
                                                                   Style::LeftIconSize,
                                                                   Style::LeftFontSize,
                                                                   iconlist.at(functionnamelist.indexOf(classificationbtnlist.at(i))),
                                                                   iconlightlist.at(functionnamelist.indexOf(classificationbtnlist.at(i))),
                                                                   ClassifyBtnHoverBackground,
                                                                   ClassifyBtnHoverBackground,
                                                                   2,
                                                                   classificationbtnlist.at(i),
                                                                   true,
                                                                   true);
        buttonList.append(iconbtn);
        iconlistscrollareawidLayout->addWidget(iconbtn);
        connect(iconbtn,SIGNAL(buttonClicked(QAbstractButton*)),pBtnGroup, SIGNAL(buttonClicked(QAbstractButton*)));
    }
    iconlistscrollareawidLayout->addItem(pIconListBottomSpacer);

    int id=0;
    Q_FOREACH (QAbstractButton* btn, buttonList) {
        pBtnGroup->addButton(btn,id++);
    }
    connect(pBtnGroup,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(btnGroupClickedSlot(QAbstractButton*)));
    iconlistscrollarea->widget()->adjustSize();
    pBtnGroup->button(0)->click();

}

void FullFunctionWidget::btnGroupClickedSlot(QAbstractButton *btn)
{
    disconnect(scrollarea->verticalScrollBar(),SIGNAL(valueChanged(int)),
            this,SLOT(valueChangedSlot(int)));
    Q_FOREACH (QAbstractButton* button, buttonList) {

        FunctionClassifyButton* fcbutton=qobject_cast<FunctionClassifyButton*>(button);
        QLayoutItem* iconitem=fcbutton->layout()->itemAt(0);
        QLabel* iconlabel=qobject_cast<QLabel*>(iconitem->widget());
        QLayoutItem* textitem=fcbutton->layout()->itemAt(1);
        QLabel* textlabel=qobject_cast<QLabel*>(textitem->widget());

        if(pBtnGroup->id(btn)==buttonList.indexOf(button))
        {
            int num=classificationbtnlist.indexOf(textlabel->text());
            if(num!=-1)
            {
                beforebtnPos=scrollarea->verticalScrollBar()->sliderPosition();
                endPos=classificationbtnrowlist.at(num).toInt();
                timer->start(1);
//                scrollarea->verticalScrollBar()->setSliderPosition(pos);
            }

            QSvgRenderer* svgRender = new QSvgRenderer;
            svgRender->load(iconlightlist.at(functionnamelist.indexOf(textlabel->text())));
            QPixmap* pixmap = new QPixmap(Style::LeftIconSize,Style::LeftIconSize);
            pixmap->fill(Qt::transparent);
            QPainter p(pixmap);
            svgRender->render(&p);
            iconlabel->setPixmap(*pixmap);
            textlabel->setStyleSheet("background:transparent;color:rgba(255, 255, 255);");
            fcbutton->is_pressed=true;

        }
        else{
            QSvgRenderer* svgRender = new QSvgRenderer;
            svgRender->load(iconlist.at(functionnamelist.indexOf(textlabel->text())));
            QPixmap* pixmap = new QPixmap(Style::LeftIconSize,Style::LeftIconSize);
            pixmap->fill(Qt::transparent);
            QPainter p(pixmap);
            svgRender->render(&p);
            iconlabel->setPixmap(*pixmap);
            textlabel->setStyleSheet("background:transparent;color:rgba(255, 255, 255,50%);");
            fcbutton->is_pressed=false;
        }
    }
}

void FullFunctionWidget::timeOutSlot()
{
    if(beginPos<endPos)
    {
        if(endPos-scrollarea->verticalScrollBar()->sliderPosition()<50)
            scrollarea->verticalScrollBar()->setSliderPosition(endPos);
        else
            scrollarea->verticalScrollBar()->setSliderPosition(scrollarea->verticalScrollBar()->sliderPosition()+50);
    }
    else
    {
        if(scrollarea->verticalScrollBar()->sliderPosition()-endPos<50)
            scrollarea->verticalScrollBar()->setSliderPosition(endPos);
        else
            scrollarea->verticalScrollBar()->setSliderPosition(scrollarea->verticalScrollBar()->sliderPosition()-50);
    }
    if(scrollarea->verticalScrollBar()->sliderPosition()==endPos ||
            scrollarea->verticalScrollBar()->sliderPosition()>=scrollarea->verticalScrollBar()->maximum())
    {
        timer->stop();
        connect(scrollarea->verticalScrollBar(),SIGNAL(valueChanged(int)),
                this,SLOT(valueChangedSlot(int)));
    }

}

void FullFunctionWidget::valueChangedSlot(int value)
{
    int count=0;
    while(count<classificationbtnrowlist.count()-1)
    {
        if(value>=classificationbtnrowlist.at(count).toInt() &&
                value <classificationbtnrowlist.at(count+1).toInt())
        {
            Q_FOREACH (QAbstractButton* button, buttonList) {

                FunctionClassifyButton* fcbutton=qobject_cast<FunctionClassifyButton*>(button);
                QLayoutItem* iconitem=fcbutton->layout()->itemAt(0);
                QLabel* iconlabel=qobject_cast<QLabel*>(iconitem->widget());
                QLayoutItem* textitem=fcbutton->layout()->itemAt(1);
                QLabel* textlabel=qobject_cast<QLabel*>(textitem->widget());

                if(count==buttonList.indexOf(button))
                {
                    QSvgRenderer* svgRender = new QSvgRenderer;
                    svgRender->load(iconlightlist.at(functionnamelist.indexOf(textlabel->text())));
                    QPixmap* pixmap = new QPixmap(Style::LeftIconSize,Style::LeftIconSize);
                    pixmap->fill(Qt::transparent);
                    QPainter p(pixmap);
                    svgRender->render(&p);
                    iconlabel->setPixmap(*pixmap);
                    textlabel->setStyleSheet("background:transparent;color:rgba(255, 255, 255);");
                    fcbutton->is_pressed=true;

                }
                else{
                    QSvgRenderer* svgRender = new QSvgRenderer;
                    svgRender->load(iconlist.at(functionnamelist.indexOf(textlabel->text())));
                    QPixmap* pixmap = new QPixmap(Style::LeftIconSize,Style::LeftIconSize);
                    pixmap->fill(Qt::transparent);
                    QPainter p(pixmap);
                    svgRender->render(&p);
                    iconlabel->setPixmap(*pixmap);
                    textlabel->setStyleSheet("background:transparent;color:rgba(255, 255, 255,50%);");
                    fcbutton->is_pressed=false;
                }
            }
            break;
        }
        else
            count++;
    }
    if(count==classificationbtnrowlist.count()-1 ||
            scrollarea->verticalScrollBar()->sliderPosition()>=scrollarea->verticalScrollBar()->maximum())
    {
        Q_FOREACH (QAbstractButton* button, buttonList) {

            FunctionClassifyButton* fcbutton=qobject_cast<FunctionClassifyButton*>(button);
            QLayoutItem* iconitem=fcbutton->layout()->itemAt(0);
            QLabel* iconlabel=qobject_cast<QLabel*>(iconitem->widget());
            QLayoutItem* textitem=fcbutton->layout()->itemAt(1);
            QLabel* textlabel=qobject_cast<QLabel*>(textitem->widget());

            if(classificationbtnrowlist.count()-1==buttonList.indexOf(button))
            {
                QSvgRenderer* svgRender = new QSvgRenderer;
                svgRender->load(iconlightlist.at(functionnamelist.indexOf(textlabel->text())));
                QPixmap* pixmap = new QPixmap(Style::LeftIconSize,Style::LeftIconSize);
                pixmap->fill(Qt::transparent);
                QPainter p(pixmap);
                svgRender->render(&p);
                iconlabel->setPixmap(*pixmap);
                textlabel->setStyleSheet("background:transparent;color:rgba(255, 255, 255);");
                fcbutton->is_pressed=true;

            }
            else{
                QSvgRenderer* svgRender = new QSvgRenderer;
                svgRender->load(iconlist.at(functionnamelist.indexOf(textlabel->text())));
                QPixmap* pixmap = new QPixmap(Style::LeftIconSize,Style::LeftIconSize);
                pixmap->fill(Qt::transparent);
                QPainter p(pixmap);
                svgRender->render(&p);
                iconlabel->setPixmap(*pixmap);
                textlabel->setStyleSheet("background:transparent;color:rgba(255, 255, 255,50%);");
                fcbutton->is_pressed=false;
            }
        }
    }
    iconlistscrollarea->verticalScrollBar()->setSliderPosition(buttonList.at(count)->pos().y());
}

void FullFunctionWidget::widgetMakeZero()
{
    Q_FOREACH (QAbstractButton* button, buttonList) {
        int num=classificationbtnlist.indexOf(functionnamelist.at(buttonList.indexOf(button)));
        if(num!=-1)
        {
            pBtnGroup->button(num)->click();
            iconlistscrollarea->verticalScrollBar()->setSliderPosition(0);
            break;
        }
    }
}
