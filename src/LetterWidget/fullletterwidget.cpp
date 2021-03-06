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

#include "fullletterwidget.h"
#include "ui_fullletterwidget.h"
#include <QDebug>

FullLetterWidget::FullLetterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FullLetterWidget)
{
    ui->setupUi(this);

    initWidget();
}

FullLetterWidget::~FullLetterWidget()
{
    delete ui;
    delete pUkuiMenuInterface;
}

/**
 * 主界面初始化
 */
void FullLetterWidget::initWidget()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setStyleSheet("border:0px;background:transparent;");
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    applistWid=new QWidget(this);
    letterlistWid=new QWidget(this);
    this->setFixedSize(Style::MainViewWidWidth,
                       Style::AppListWidHeight);
    applistWid->setFixedSize(Style::AppListWidWidth,this->height());
    letterlistWid->setFixedSize(Style::LeftWidWidth,this->height());

    mainLayout=new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    applistWid->setStyleSheet("border:0px;background: transparent;");
    letterlistWid->setStyleSheet("border:0px;background: transparent;");
    mainLayout->addWidget(letterlistWid);
    mainLayout->addWidget(applistWid);
    this->setLayout(mainLayout);
    pUkuiMenuInterface=new UkuiMenuInterface;

    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeOutSlot()));

    initAppListWidget();
    initLetterListWidget();
}

/**
 * 初始化应用列表界面
 */
void FullLetterWidget::initAppListWidget()
{
    QHBoxLayout* layout=new QHBoxLayout(applistWid);
    layout->setContentsMargins(0,0,0,0);
    applistWid->setLayout(layout);

    scrollarea=new ScrollArea;
    scrollareawid=new QWidget;
    scrollarea->setWidget(scrollareawid);
    scrollarea->setFixedSize(applistWid->width(),applistWid->height());
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
void FullLetterWidget::fillAppList()
{
    letterbtnlist.clear();
    letterbtnrowlist.clear();

    QVector<QStringList> vector=pUkuiMenuInterface->getAlphabeticClassification();
    for(int i=0;i<vector.size();i++)
    {
        QStringList appList=vector.at(i);
        if(!appList.isEmpty())
        {
            QString letterstr;
            if(i<26)
                letterstr=QString(QChar(static_cast<char>(i+65)));
            else if(i==26)
                letterstr="&";
            else
                letterstr="#";
            letterbtnlist.append(letterstr);//存储分类字符
            //插入字母分类按钮
            PushButton* letterbtn=new PushButton(letterstr,scrollarea->width()-12,20);
//            letterbtn->setFixedSize(scrollarea->width(),20);
            scrollareawidLayout->addWidget(letterbtn);

            //插入应用列表
            FullListView* listview=new FullListView(this,1);
            scrollareawidLayout->addWidget(listview);
            data.clear();
            for(int i=0;i<appList.count();i++)
            {
                QString desktopfp=pUkuiMenuInterface->getDesktopPathByAppName(appList.at(i));
                data.append(desktopfp);

            }
            listview->addData(data);

            connect(listview,SIGNAL(sendItemClickedSignal(QString)),this,SLOT(execApplication(QString)));
            connect(listview,SIGNAL(sendFixedOrUnfixedSignal()),this,SIGNAL(sendUpdateAppListSignal()));
            connect(listview,SIGNAL(sendHideMainWindowSignal()),this,SIGNAL(sendHideMainWindowSignal()));
        }
    }

        resizeScrollAreaControls();
}

/**
 * 执行应用程序
 */
void FullLetterWidget::execApplication(QString appname)
{
    qDebug()<<appname;
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
void FullLetterWidget::updateAppListView()
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

    letterbtnlist.clear();
    letterbtnrowlist.clear();

    QVector<QStringList> vector=pUkuiMenuInterface->getAlphabeticClassification();
    for(int i=0;i<vector.size();i++)
    {
        QStringList appList=vector.at(i);
        if(!appList.isEmpty())
        {
            QString letterstr;
            if(i<26)
                letterstr=QString(QChar(static_cast<char>(i+65)));
            else if(i==26)
                letterstr="&";
            else
                letterstr="#";
            letterbtnlist.append(letterstr);//存储分类字符
            //插入字母分类按钮
            PushButton* letterbtn=new PushButton(letterstr,scrollarea->width()-12,20);
//            letterbtn->setFixedSize(scrollarea->width(),20);
            scrollareawidLayout->addWidget(letterbtn);

            //插入应用列表
            FullListView* listview=new FullListView(this,1);
            scrollareawidLayout->addWidget(listview);
            data.clear();
            for(int i=0;i<appList.count();i++)
            {
                QString desktopfp=pUkuiMenuInterface->getDesktopPathByAppName(appList.at(i));
                data.append(desktopfp);

            }
            listview->updateData(data);

            connect(listview,SIGNAL(sendItemClickedSignal(QString)),this,SLOT(execApplication(QString)));
            connect(listview,SIGNAL(sendFixedOrUnfixedSignal()),this,SIGNAL(sendUpdateAppListSignal()));
            connect(listview,SIGNAL(sendHideMainWindowSignal()),this,SIGNAL(sendHideMainWindowSignal()));
        }
    }

        resizeScrollAreaControls();

    //刷新字母列表界面
    Q_FOREACH (QAbstractButton* button, buttonList) {
        pBtnGroup->removeButton(button);
    }
    buttonList.clear();

    for(int index=letterlistscrollareawidLayout->count()-1;index>=0;index--)
    {
        QLayoutItem* item=letterlistscrollareawidLayout->takeAt(index);
        QWidget* wid=item->widget();
        letterlistscrollareawidLayout->removeWidget(wid);
        wid->setParent(nullptr);
        delete wid;
    }
    char btnstyle[500];
    sprintf(btnstyle,"QToolButton{background:transparent;color:#8b8b8b;font-size:14px;padding-left:0px;}\
            QToolButton:hover{background-color:%s;color:#ffffff;font-size:14px;}\
            QToolButton:pressed{background-color:%s;color:#8b8b8b;font-size:14px;}\
            QToolButton:checked{background:transparent;color:#ffffff;font-size:14px;}",
            ClassifyBtnHoverBackground,ClassifyBtnHoverBackground);

    QStringList letterbtnlist=this->letterbtnlist;
    if(letterbtnlist.contains("&"))
        letterbtnlist.replace(letterbtnlist.indexOf("&"),"&&");
    for(int i=0;i<letterbtnlist.size();i++)
    {
        QToolButton* letterbtn=new QToolButton;
        letterbtn->setText(letterbtnlist.at(i));
        letterbtn->setStyleSheet(btnstyle);
        letterbtn->setFixedSize(48,48);
        letterbtn->setCheckable(true);
        buttonList.append(letterbtn);
        letterlistscrollareawidLayout->addWidget(letterbtn);
    }
    int id=0;
    Q_FOREACH (QAbstractButton* btn, buttonList) {
        pBtnGroup->addButton(btn,id++);
    }

    letterlistscrollarea->widget()->adjustSize();
    pBtnGroup->button(0)->click();
}

/**
 * 设置scrollarea所填充控件大小
 */
void FullLetterWidget::resizeScrollAreaControls()
{
    int pos=0;
    letterbtnrowlist.append(QString::number(pos));
    int row=0;
    while(row<scrollareawidLayout->count()/2)
    {
        //分类按钮
//        QLayoutItem* letterbtnwidItem=scrollareawidLayout->itemAt(row*2);
//        QWidget* letterbtnwid=letterbtnwidItem->widget();
//        QPushButton* letterbtn=qobject_cast<QPushButton*>(letterbtnwid);
//        letterbtn->setFixedSize(scrollarea->width()-32,20);
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

//        listview->setFixedSize(scrollarea->width()-12,listview->gridSize().height()*rowcount);
        listview->setFixedSize(scrollarea->width()-Style::SliderSize+1,listview->gridSize().height()*rowcount);
        if(row<scrollareawidLayout->count()/2-1)
        {
            pos+=(20+listview->height());
            letterbtnrowlist.append(QString::number(pos));
        }
        row++;
    }
    scrollarea->widget()->adjustSize();
}

/**
 * 初始化字母列表界面
 */
void FullLetterWidget::initLetterListWidget()
{
    letterlistleftSpacer=new QSpacerItem(40,20,QSizePolicy::Expanding,QSizePolicy::Fixed);
    letterlistrightSpacer=new QSpacerItem(40,20,QSizePolicy::Expanding,QSizePolicy::Fixed);

    letterlistLayout=new QHBoxLayout(letterlistWid);
    letterlistLayout->setContentsMargins(Style::LeftMargin,0,Style::RightMargin,0);
    letterlistLayout->setSpacing(0);
    letterlistWid->setLayout(letterlistLayout);

    letterlistscrollarea=new ClassifyScrollArea();
    letterlistscrollarea->setFixedSize(Style::LeftBtnHeight,letterlistWid->height());
    letterlistscrollareaWid=new QWidget;
    letterlistscrollareawidLayout=new QVBoxLayout;
    letterlistscrollareawidLayout->setContentsMargins(0,0,0,0);
    letterlistscrollareawidLayout->setSpacing(Style::LeftSpaceBetweenItem);
    letterlistscrollareaWid->setLayout(letterlistscrollareawidLayout);
    letterlistscrollarea->setWidget(letterlistscrollareaWid);
    letterlistscrollarea->setWidgetResizable(true);

    letterlistLayout->addWidget(letterlistscrollarea);
    pBtnGroup=new QButtonGroup(letterlistscrollareaWid);

    initLetterListScrollArea();
}

/**
 * 初始化字母列表
 */
void FullLetterWidget::initLetterListScrollArea()
{
    char btnstyle[500];
    sprintf(btnstyle,"QToolButton{background:transparent;color:#8b8b8b;padding-left:0px;border-radius:2px;}\
            QToolButton:hover{background-color:%s;color:#ffffff;border-radius:2px;}\
            QToolButton:pressed{background-color:%s;color:#8b8b8b;border-radius:2px;}\
            QToolButton:checked{background:transparent;color:#ffffff;border-radius:2px;}",
            ClassifyBtnHoverBackground,ClassifyBtnHoverBackground);

    QFont font;
    font.setPixelSize(Style::LeftFontSize);

    QStringList letterbtnlist=this->letterbtnlist;
    if(letterbtnlist.contains("&"))
        letterbtnlist.replace(letterbtnlist.indexOf("&"),"&&");
    for(int i=0;i<letterbtnlist.size();i++)
    {
        QToolButton* letterbtn=new QToolButton;
        letterbtn->setText(letterbtnlist.at(i));
        letterbtn->setStyleSheet(btnstyle);
        letterbtn->setFixedSize(Style::LeftBtnHeight,Style::LeftBtnHeight);
        letterbtn->setCheckable(true);
        letterbtn->setFont(font);
        buttonList.append(letterbtn);
        letterlistscrollareawidLayout->addWidget(letterbtn);
//        connect(letterbtn,SIGNAL(clicked()),this,SLOT(letterBtnClickedSlot()));
    }
    int id=0;
    Q_FOREACH (QAbstractButton* btn, buttonList) {
        pBtnGroup->addButton(btn,id++);
    }

    connect(pBtnGroup,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(btnGroupClickedSlot(QAbstractButton*)));
    letterlistscrollarea->widget()->adjustSize();
    pBtnGroup->button(0)->click();
}

void FullLetterWidget::btnGroupClickedSlot(QAbstractButton *btn)
{
    disconnect(scrollarea->verticalScrollBar(),SIGNAL(valueChanged(int)),
            this,SLOT(valueChangedSlot(int)));
    Q_FOREACH (QAbstractButton* button, buttonList) {
        if(pBtnGroup->id(btn)==buttonList.indexOf(button))
        {
            button->setChecked(true);
            //此处需实现将被选定的字母包含的应用列表移动到applistWid界面最顶端
            QString letterstr=button->text().at(0);
            int num=letterbtnlist.indexOf(letterstr);
            if(num!=-1)
            {
                beginPos=scrollarea->verticalScrollBar()->sliderPosition();
                endPos=letterbtnrowlist.at(num).toInt();
                timer->start(1);
//                scrollarea->verticalScrollBar()->setSliderPosition(pos);
            }
        }
        else{
            button->setChecked(false);
        }
    }
}

void FullLetterWidget::timeOutSlot()
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

void FullLetterWidget::valueChangedSlot(int value)
{
    int count=0;
    while(count<letterbtnrowlist.count()-1)
    {
        if(value>=letterbtnrowlist.at(count).toInt() &&
                value <letterbtnrowlist.at(count+1).toInt())
        {
            buttonList.at(count)->setChecked(true);
            break;
        }
        else
            count++;
    }
    if(count==letterbtnrowlist.count()-1 ||
            scrollarea->verticalScrollBar()->sliderPosition()==scrollarea->verticalScrollBar()->maximum())
    {
        buttonList.at(letterbtnrowlist.count()-1)->setChecked(true);
        letterlistscrollarea->verticalScrollBar()->setSliderPosition(letterlistscrollarea->verticalScrollBar()->maximum());

    }

    //向下滚动
    if((buttonList.at(count)->pos().y()+buttonList.at(count)->height()+letterlistscrollarea->widget()->pos().y()) >= letterlistscrollarea->height())
    {
        int val=letterlistscrollarea->verticalScrollBar()->sliderPosition()+buttonList.at(count)->height();
        letterlistscrollarea->verticalScrollBar()->setSliderPosition(val);
    }

    //向上滚动
    if((buttonList.at(count)->pos().y()+letterlistscrollarea->widget()->pos().y()) <= 0)
    {

        int val=letterlistscrollarea->verticalScrollBar()->value()-buttonList.at(count)->height();
        letterlistscrollarea->verticalScrollBar()->setSliderPosition(val);
    }

}

void FullLetterWidget::widgetMakeZero()
{
    Q_FOREACH (QAbstractButton* button, buttonList) {
        QString letterstr=button->text().at(0);
        int num=letterbtnlist.indexOf(letterstr);
        if(num!=-1)
        {
            pBtnGroup->button(num)->click();
            letterlistscrollarea->verticalScrollBar()->setSliderPosition(0);
            break;
        }
    }
}
