/*
 * Copyright (c) 2012 Mark Liversedge (liversedge@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "GcCalendar.h"
#include <QWebSettings>
#include <QWebFrame>
#include <TimeUtils.h>

GcCalendar::GcCalendar(MainWindow *main) : main(main)
{
    setContentsMargins(0,0,0,0);
    setAutoFillBackground(true);

    month = year = 0;
    _ride = NULL;

    layout = new QVBoxLayout(this);
    layout->setSpacing(0);

    black.setColor(QPalette::WindowText, QColor(0,0,0,240));
    white.setColor(QPalette::WindowText, QColor(255,255,255,240));
    grey.setColor(QPalette::WindowText, QColor(127,127,127,255));

    // get the model
    fieldDefinitions = main->rideMetadata()->getFields();
    calendarModel = new GcCalendarModel(this, &fieldDefinitions, main);
    calendarModel->setSourceModel(main->listView->sqlModel);

    QFont font;
    font.setPointSize(48);
    dayNumber = new GcLabel("4", this);
    dayNumber->setYOff(-1);
    dayNumber->setAutoFillBackground(false);
    dayNumber->setPalette(white);
    dayNumber->setFont(font);
    dayNumber->setAlignment(Qt::AlignLeft);
    layout->addWidget(dayNumber);

    font.setPointSize(24);
    dayName = new GcLabel("Sunday", this);
    dayName->setYOff(-1);
    dayName->setAutoFillBackground(false);
    dayName->setPalette(white);
    dayName->setFont(font);
    dayName->setAlignment(Qt::AlignLeft);
    layout->addWidget(dayName);

    font.setPointSize(12);
    font.setWeight(QFont::Bold);
    dayDate = new GcLabel("4th January 2012", this);
    dayDate->setAutoFillBackground(false);
    dayDate->setPalette(grey);
    dayDate->setFont(font);
    dayDate->setAlignment(Qt::AlignLeft);
    layout->addWidget(dayDate);

    GcLabel *spacer1 = new GcLabel("", this);
    spacer1->setFixedHeight(20);
    layout->addWidget(spacer1);

    QHBoxLayout *line = new QHBoxLayout;
    line->setSpacing(5);
    layout->addLayout(line);

    GcLabel *spacer2 = new GcLabel("", this);
    spacer2->setFixedHeight(10);
    layout->addWidget(spacer2);

    font.setPointSize(12);
    left = new GcLabel("<", this);
    left->setYOff(1);
    left->setAutoFillBackground(false);
    left->setPalette(white);
    left->setFont(font);
    left->setAlignment(Qt::AlignLeft);
    line->addWidget(left);
    connect (left, SIGNAL(clicked()), this, SLOT(previous()));

    font.setPointSize(12);
    monthName = new GcLabel("January 2012", this);
    monthName->setYOff(1);
    monthName->setAutoFillBackground(false);
    monthName->setPalette(white);
    monthName->setFont(font);
    monthName->setAlignment(Qt::AlignCenter);
    line->addWidget(monthName);

    font.setPointSize(12);
    right = new GcLabel(">", this);
    right->setYOff(1);
    right->setAutoFillBackground(false);
    right->setPalette(white);
    right->setFont(font);
    right->setAlignment(Qt::AlignRight);
    line->addWidget(right);
    connect (right, SIGNAL(clicked()), this, SLOT(next()));

    dayLayout = new QGridLayout;
    dayLayout->setSpacing(1);
    layout->addLayout(dayLayout);

    font.setWeight(QFont::Normal);

    // Mon
    font.setPointSize(9);
    GcLabel *day = new GcLabel("Mon", this);
    day->setFont(font);
    day->setAutoFillBackground(false);
    day->setPalette(white);
    day->setFont(font);
    day->setAlignment(Qt::AlignCenter);
    dayLayout->addWidget(day, 0, 0);

    // Tue
    day = new GcLabel("Tue", this);
    day->setFont(font);
    day->setAutoFillBackground(false);
    day->setPalette(white);
    day->setFont(font);
    day->setAlignment(Qt::AlignCenter);
    dayLayout->addWidget(day, 0, 1);

    // Wed
    day = new GcLabel("Wed", this);
    day->setFont(font);
    day->setAutoFillBackground(false);
    day->setPalette(white);
    day->setFont(font);
    day->setAlignment(Qt::AlignCenter);
    dayLayout->addWidget(day, 0, 2);

    // Thu
    day = new GcLabel("Thu", this);
    day->setFont(font);
    day->setAutoFillBackground(false);
    day->setPalette(white);
    day->setFont(font);
    day->setAlignment(Qt::AlignCenter);
    dayLayout->addWidget(day, 0, 3);

    // Fri
    day = new GcLabel("Fri", this);
    day->setFont(font);
    day->setAutoFillBackground(false);
    day->setPalette(white);
    day->setFont(font);
    day->setAlignment(Qt::AlignCenter);
    dayLayout->addWidget(day, 0, 4);

    // Sat
    day = new GcLabel("Sat", this);
    day->setFont(font);
    day->setAutoFillBackground(false);
    day->setPalette(white);
    day->setFont(font);
    day->setAlignment(Qt::AlignCenter);
    dayLayout->addWidget(day, 0, 5);

    // Sun
    day = new GcLabel("Sun", this);
    day->setFont(font);
    day->setAutoFillBackground(false);
    day->setPalette(white);
    day->setFont(font);
    day->setAlignment(Qt::AlignCenter);
    dayLayout->addWidget(day, 0, 6);

    signalMapper = new QSignalMapper(this);
    font.setPointSize(11);
    for (int row=1; row<7; row++) {

        for (int col=0; col < 7; col++) {

            GcLabel *d = new GcLabel(QString("%1").arg((row-1)*7+col), this);
            d->setFont(font);
            d->setAutoFillBackground(false);
            d->setPalette(white);
            d->setAlignment(Qt::AlignCenter);
            dayLayout->addWidget(d,row,col);

            // we like squares
            d->setFixedHeight(28);
            d->setFixedWidth(28);

            dayLabels << d;

            if (row== 3 && col == 4) d->setSelected(true);

            connect (d, SIGNAL(clicked()), signalMapper, SLOT(map()));
            signalMapper->setMapping(d, (row-1)*7+col);
        }
    }

    // Summary level selector
    QHBoxLayout *h = new QHBoxLayout();
    h->setSpacing(5);
    summarySelect = new QComboBox(this);
    //summarySelect->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength); XXX
    summarySelect->setFixedWidth(150); // XXX is it impossible to size constrain qcombos?????
    summarySelect->addItem("Day Summary");
    summarySelect->addItem("Weekly Summary");
    summarySelect->addItem("Monthly Summary");
    summarySelect->setCurrentIndex(1); // default to weekly
    h->addStretch();
    h->addWidget(summarySelect, Qt::AlignCenter);
    h->addStretch();
    layout->addLayout(h);
    layout->addStretch();

    summary = new QWebView(this);
    summary->setContentsMargins(0,0,0,0);
    summary->page()->view()->setContentsMargins(0,0,0,0);
    summary->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    summary->setAcceptDrops(false);

    QFont defaultFont; // mainwindow sets up the defaults.. we need to apply
    summary->settings()->setFontSize(QWebSettings::DefaultFontSize, defaultFont.pointSize()+1);
    summary->settings()->setFontFamily(QWebSettings::StandardFont, defaultFont.family());
    layout->addWidget(summary);

    // summary mode changed
    connect(summarySelect, SIGNAL(currentIndexChanged(int)), this, SLOT(refresh()));

    // day clicked
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(dayClicked(int)));

    // refresh on these events...
    connect(main, SIGNAL(rideAdded(RideItem*)), this, SLOT(refresh()));
    connect(main, SIGNAL(rideDeleted(RideItem*)), this, SLOT(refresh()));
    connect(main, SIGNAL(configChanged()), this, SLOT(refresh()));

    // set up for current selections
    refresh();
}

void
GcCalendar::refresh()
{
    calendarModel->setMonth(month, year);
    setSummary();
    repaint();
}

bool
GcCalendar::event(QEvent *e)
{
    if (e->type() != QEvent::ToolTip && e->type() != QEvent::Paint && e->type() != QEvent::Destroy &&
        e->type() != QEvent::LayoutRequest) {
        main->setBubble("");
        //qDebug()<<"event"<<e->type();
    }

    if (e->type() == QEvent::Paint) {
        // fill the background
        QPainter painter(this);
        QRect all(0,0,width(),height());
        //painter.fillRect(all, QColor("#B3B4BA"));
        painter.fillRect(all, QColor(Qt::white));
    }

    int n=0;
    if (e->type() == QEvent::ToolTip) {

        // are we hovering over a label?
        foreach(GcLabel *label, dayLabels) {
            if (label->underMouse()) {
                if (dayLabels.at(n)->text() == "") return false;

                // select a ride if there is one for this one?
                int row = n / 7;
                int col = n % 7;

                QModelIndex p = calendarModel->index(row,col);
                QStringList files = calendarModel->data(p, GcCalendarModel::FilenamesRole).toStringList();

                QPoint pos = dynamic_cast<QHelpEvent*>(e)->pos();

                // Popup bubble for ride
                if (files.count()) {
                    if (files[0] == "calendar") ; // XXX handle planned rides
                    else main->setBubble(files.at(0), mapToGlobal(pos+QPoint(-2,-2)));
                }
            }
            n++;
        }
    }
    return QWidget::event(e);
}

void
GcCalendar::dayClicked(int i)
{
    if (dayLabels.at(i)->text() == "") return;

    // select a ride if there is one for this one?
    int row = i / 7;
    int col = i % 7;


    QModelIndex p = calendarModel->index(row,col);
    QStringList files = calendarModel->data(p, GcCalendarModel::FilenamesRole).toStringList();

    if (files.count()) // XXX if more than one file cycle through them?
        main->selectRideFile(QFileInfo(files[0]).fileName());

}

void
GcCalendar::previous()
{
    QList<QDateTime> allDates = main->metricDB->db()->getAllDates();
    qSort(allDates);

    // begin of month
    QDateTime bom(QDate(year,month,01), QTime(0,0,0));
    for(int i=allDates.count()-1; i>0; i--) {
        if (allDates.at(i) < bom) {

            QDate date = allDates.at(i).date();
            calendarModel->setMonth(date.month(), date.year());

            // find the day in the calendar...
            for (int day=42; day>0;day--) {

                QModelIndex p = calendarModel->index(day/7,day%7);
                QDate heredate = calendarModel->date(p);
                if (date == heredate) {
                    // select this ride...
                    QStringList files = calendarModel->data(p, GcCalendarModel::FilenamesRole).toStringList();
                    if (files.count()) main->selectRideFile(QFileInfo(files[0]).fileName());
                }
            }
            break;
        }
    }
}

void
GcCalendar::next()
{
    QList<QDateTime> allDates = main->metricDB->db()->getAllDates();
    qSort(allDates);

    // end of month
    QDateTime eom(QDate(year,month,01).addMonths(1), QTime(00,00,00));
    for(int i=0; i<allDates.count(); i++) {
        if (allDates.at(i) >= eom) {

            QDate date = allDates.at(i).date();
            calendarModel->setMonth(date.month(), date.year());

            // find the day in the calendar...
            for (int day=0; day<42;day++) {

                QModelIndex p = calendarModel->index(day/7,day%7);
                QDate heredate = calendarModel->date(p);
                if (date == heredate) {
                    // select this ride...
                    QStringList files = calendarModel->data(p, GcCalendarModel::FilenamesRole).toStringList();
                    if (files.count()) main->selectRideFile(QFileInfo(files[0]).fileName());
                }
            }
            break;
        }
    }
}

void
GcCalendar::setRide(RideItem *ride)
{
    _ride = ride;

    QDate when;
    if (_ride && _ride->ride()) when = _ride->dateTime.date();
    else when = QDate::currentDate();

    // refresh the model
    if (when.month() != month || when.year() != year) {
        calendarModel->setMonth(when.month(), when.year());
        year = when.year();
        month = when.month();

        monthName->setText(when.toString("MMMM yyyy"));
    }

    dayNumber->setText(when.toString("d"));
    dayName->setText(when.toString("dddd"));
    dayDate->setText(when.toString("d MMMM yyyy"));

    // now reapply for each row/col of calendar...
    for (int row=0; row<6; row++) {
        for (int col=0; col < 7; col++) {

            GcLabel *d = dayLabels.at(row*7+col);
            QModelIndex p = calendarModel->index(row,col);
            QDate date = calendarModel->date(p);

            if (date.month() != month || date.year() != year) {
                d->setText("");
                d->setBg(false);
                d->setSelected(false);
            } else {
                d->setText(QString("%1").arg(date.day()));

                // what color should it be?
                // for taste we /currently/ just set to bg to
                // highlight there is a ride there, colors /will/ come
                // back later when worked out a way of making it look
                // nice and not garish 
                QList<QColor> colors = p.data(Qt::BackgroundRole).value<QList<QColor> >();
                if (colors.count()) {
                    d->setBg(true);
                    d->setPalette(black);
                    d->setBgColor(colors.at(0)); // use first always
                } else {
                    d->setBg(false);
                    d->setPalette(white);
                }
                if (date == when) {
                    d->setSelected(true);
                    d->setPalette(white);
                } else d->setSelected(false);
            }
        }
    }
    refresh();
}

void
GcLabel::paintEvent(QPaintEvent *e)
{
    int x,y,w,l;

    if (bg) {
        // setup a painter and the area to paint
        QPainter painter(this);
        QRect all(0,0,width(),height());
        painter.fillRect(all, bgColor);
        painter.drawRect(QRect(0,0,width()-1,height()-1));
    }

    if (selected) {
        QPainter painter(this);
        QRect all(0,0,width(),height());
        painter.fillRect(all, GColor(CCALCURRENT));
    }

    if (xoff || yoff) {
        // save settings
        QPalette p = palette();
        getContentsMargins(&x,&y,&w,&l);

        // adjust for emboss
        setContentsMargins(x+xoff,y+yoff,w,l);
        QPalette r;
        r.setColor(QPalette::WindowText, QColor(127,127,127,160));
        setPalette(r);
        QLabel::paintEvent(e);

        // Now normal
        setContentsMargins(x,y,w,l);
        setPalette(p);
    }

    QLabel::paintEvent(e);
}

void
GcCalendar::setSummary()
{
    // are we metric?
    bool useMetricUnits = (appsettings->value(this, GC_UNIT).toString() == "Metric");

    // where we construct the text
    QString summaryText("");

    QDate when;
    if (_ride && _ride->ride()) when = _ride->dateTime.date();
    else when = QDate::currentDate();

    // main totals
    static const QStringList totalColumn = QStringList()
        << "workout_time"
        << "time_riding"
        << "total_distance"
        << "total_work"
        << "elevation_gain";

    static const QStringList averageColumn = QStringList()
        << "average_speed"
        << "average_power"
        << "average_hr"
        << "average_cad";

    static const QStringList maximumColumn = QStringList()
        << "max_speed"
        << "max_power"
        << "max_heartrate"
        << "max_cadence";

    // user defined
    QString s = appsettings->value(this, GC_SETTINGS_SUMMARY_METRICS, GC_SETTINGS_SUMMARY_METRICS_DEFAULT).toString();

    // in case they were set tand then unset
    if (s == "") s = GC_SETTINGS_SUMMARY_METRICS_DEFAULT;
    QStringList metricColumn = s.split(",");

    // what date range should we use?
    QDate newFrom, newTo;

    switch(summarySelect->currentIndex()) {

        case 0 :
            // DAILY - just the date of the ride
            newFrom = newTo = when;
            break;
        case 1 :
            // WEEKLY - from Mon to Sun
            newFrom = when.addDays((when.dayOfWeek()-1)*-1);
            newTo = newFrom.addDays(6);
            break;

        default:
        case 2 : 
            // MONTHLY - all days in month
            newFrom = QDate(when.year(), when.month(), 1);
            newTo = newFrom.addMonths(1).addDays(-1);
            break;
    }

    if (newFrom == from && newTo == to) return;
    else {

        // date range changed lets refresh
        from = newFrom;
        to = newTo;

        // lets get the metrics
        QList<SummaryMetrics>results = main->metricDB->getAllMetricsFor(QDateTime(from,QTime(0,0,0)), QDateTime(to, QTime(24,59,59)));

        // foreach of the metrics get an aggregated value
        // header of summary
        summaryText = QString("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2//EN\">"
                              "<html>"
                              "<head>"
                              "<title></title>"
                              "</head>"
                              "<body>"
                              "<center>");

        for (int i=0; i<3; i++) { //XXX taken out maximums -- too much info -- looks ugly

            QString aggname;
            QStringList list;

            switch(i) {
                case 0 : // Totals
                    aggname = "Totals";
                    list = totalColumn;
                    break;

                case 1 :  // Averages
                    aggname = "Averages";
                    list = averageColumn;
                    break;

                case 3 :  // Maximums
                    aggname = "Maximums";
                    list = maximumColumn;
                    break;

                case 2 :  // User defined.. 
                    aggname = "Metrics";
                    list = metricColumn;
                    break;

            }

            summaryText += QString("<p><table width=\"85%\">"
                                   "<tr>"
                                   "<td align=\"center\" colspan=\"2\">"
                                   "<b>%1</b>"
                                   "</td>"
                                   "</tr>").arg(aggname);

            foreach(QString metricname, list) {

                const RideMetric *metric = RideMetricFactory::instance().rideMetric(metricname);

                QString value = getAggregated(metricname, results);


                // don't show units for time values
                if (metric && (metric->units(useMetricUnits) == "seconds" ||
                               metric->units(useMetricUnits) == "")) {

                    summaryText += QString("<tr><td>%1:</td><td align=\"right\"> %2</td>")
                                            .arg(metric ? metric->name() : "unknown")
                                            .arg(value);

                } else {
                    summaryText += QString("<tr><td>%1(%2):</td><td align=\"right\"> %3</td>")
                                            .arg(metric ? metric->name() : "unknown")
                                            .arg(metric ? metric->units(useMetricUnits) : "unknown")
                                            .arg(value);
                }
            }
            summaryText += QString("</tr>" "</table>");

        }

        // finish off the html document
        summaryText += QString("</center>"
                               "</body>"
                               "</html>");

        // set webview contents
        summary->page()->mainFrame()->setHtml(summaryText);

    }
}

QString GcCalendar::getAggregated(QString name, QList<SummaryMetrics> &results)
{
    // get the metric details, so we can convert etc
    const RideMetric *metric = RideMetricFactory::instance().rideMetric(name);
    if (!metric) return QString("%1 unknown").arg(name);

    // do we need to convert from metric to imperial?
    bool useMetricUnits = (appsettings->value(this, GC_UNIT).toString() == "Metric");

    // what we will return
    double rvalue = 0;
    double rcount = 0; // using double to avoid rounding issues with int when dividing

    // loop through and aggregate
    foreach (SummaryMetrics rideMetrics, results) {

        // get this value
        double value = rideMetrics.getForSymbol(name);
        double count = rideMetrics.getForSymbol("workout_time"); // for averaging

        
        // check values are bounded, just in case
        if (isnan(value) || isinf(value)) value = 0;

        // imperial / metric conversion
        if (useMetricUnits == false) {
            value *= metric->conversion();
            value += metric->conversionSum();
        }

        switch (metric->type()) {
        case RideMetric::Total:
            rvalue += value;
            break;
        case RideMetric::Average:
            {
            // average should be calculated taking into account
            // the duration of the ride, otherwise high value but
            // short rides will skew the overall average
            rvalue += value*count;
            rcount += count;
            break;
            }
        case RideMetric::Peak:
            {
            if (value > rvalue) rvalue = value;
            break;
            }
        }
    }

    // now compute the average
    if (metric->type() == RideMetric::Average) {
        if (rcount) rvalue = rvalue / rcount;
    }


    // Format appropriately
    QString result;
    if (metric->units(useMetricUnits) == "seconds") result = time_to_string(rvalue);
    else result = QString("%1").arg(rvalue, 0, 'f', metric->precision());

    return result;
}
