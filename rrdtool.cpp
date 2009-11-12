#include "rrdtool.h"

/*
rrdtool create $DB --start $START-10 --step 10 DS:$STATS:GAUGE:$HB:0:U RRA:AVERAGE:0.5:1:1000000
rrdtool - < $RRD_DIR/_rrd_update
rrdtool graph $PIC -s $START -e $END -S $STEP -h 400 -w 10000 \
                        -x MINUTE:10:HOUR:4:HOUR:2:0:%d/%m" "%H:%M \
            DEF:linea=$DB:$STATS:AVERAGE LINE1:linea#FF0000:"Line A"
*/

/*
In a database we have more data sources.
In each data source we enter primary data points.
Those primary data points will go through a consolidation function and this value will go to RRAs

we have 3 times that matter:
    - step from database: Specifies the base interval in seconds with which data will be fed into the RRD.
    - heartbeat from datasource : defines the maximum number of seconds that may pass between two updates
            of this data source before the value of the data source is assumed to be *UNKNOWN*
   - steps from rra (NUMBER): defines how many of these primary data points are used to build a consolidated data point
                which then goes into the archive.
*/
RRDTool::RRDTool()
{
    MY_DEBUG;

    stringArguments.clear();
    //threadStatus = false;

    process = new QProcess(this);
    inforrd = RRDInfo::instance();
    message = Messages::instance();

    connect(process, SIGNAL(finished(int)),
            this,SLOT(finish()));
    connect(process, SIGNAL(started()),
            this,SLOT(start()));
}

void RRDTool::finish()
{
    MY_DEBUG;

    process->waitForFinished();

    stringOutput.clear(); stringOutput = process->readAllStandardOutput();
    stringError.clear(); stringError = process->readAllStandardError();

    message->sendLog("Time spent in rrd " + here + " was: " + QString::number((double)time.elapsed()/1000));

    here = "";
    emit finished();
}

void RRDTool::start()
{
    MY_DEBUG;

    emit started();
}

void RRDTool::run()
{
    MY_DEBUG;

    stringExpectedError.clear();
    stringExpectedOutput.clear();
#if defined (Q_OS_WIN32)
    process->setWorkingDirectory(stringDirCurrent + "/rrd");
#endif

    process->start(stringProgramRRD, stringArguments);
}

/*USAGE:
rrdtool create filename [--start|-b start time] [--step|-s step] [DS:ds-name:DST:dst arguments] [RRA:CF:cf arguments]
- create: lets you set up new Round Robin Database (RRD) files. The file is created at its final, full size
    and filled with *UNKNOWN* data.
- filename: The name of the RRD you want to create.
- start: Specifies the time in seconds since 1970-01-01 UTC
- step: Specifies the base interval in seconds with which data will be fed into the RRD.
- DS: A single RRD can accept input from several data sources
- DST: defines the Data Source Type.
    GAUGE | COUNTER | DERIVE | ABSOLUTE
        GAUGE: is for things like temperatures or number of people in a room. Can increase or decrease
        COUNTER, DERIVE: is for continuous incrementing counters like the ifInOctets counter in a router
- dst arguments: heartbeat:min:max
        heartbeat: defines the maximum number of seconds that may pass between two updates of this data source
            before the value of the data source is assumed to be *UNKNOWN*.
        min, max: define the expected range values for data supplied by a data source. U for unknown.
- RRA: round robin archives
- CF: consolidation function: AVERAGE, MIN, MAX, LAST
    AVERAGE: the average of the data points is stored.
    MIN: the smallest of the data points is stored
    MAX: the largest of the data points is stored
    LAST: the last data points is used
- cf arguments: xff:steps:rows
    xff: The xfiles factor defines what part of a consolidation interval may be made up from *UNKNOWN* data
         while the consolidated value is still regarded as known. It is given as the ratio of allowed *UNKNOWN*
         PDPs(primary data points) to the number of PDPs in the interval. Thus, it ranges from 0 to 1 (exclusive)
    steps: defines how many of these primary data points are used to build a consolidated data point
    rows: defines how many generations of data values are kept in an RRA
*/
void RRDTool::create()
{
    MY_DEBUG;

    time.start();
    here = "create";
    stringArguments.clear();

    QString datasourceconfig="", rraconfig="";
    stringArguments << "create" << inforrd->getDatabasePath() <<
            "--start" << QString::number(inforrd->getMinTime()-10) <<
            "--step" << QString::number(inforrd->getDBStep());

    //Start datasources configuration
    typeDS temp1;
    for (unsigned int i=0; i<inforrd->getDatasourcesNumber(); i++){
        temp1 = inforrd->getDatasource(i);
        datasourceconfig = "DS:" + temp1.name + ":" + temp1.type + \
                           ":" + QString::number(temp1.heartbeat) + ":" + \
                           temp1.min + ":" + temp1.max;
        stringArguments << datasourceconfig;
    }

    //Start rra configuration
    typeRRA temp2;
    for (unsigned int i=0; i<inforrd->getRRAsNumber(); i++){
        temp2 = inforrd->getRRA(i);
        rraconfig = "RRA:" + temp2.consolidationfunction + ":" + \
                    QString::number(temp2.xfilefactor) + ":" + \
                    QString::number(temp2.steps) + ":" + QString::number(temp2.rows);
        stringArguments << rraconfig;
    }

    message->sendUpdateInfo("Start creating the database.", 0);

    run();
}

/*USAGE:
rrdtool fetch filename CF [--resolution|-r resolution] [--start|-s start] [--end|-e end]
- fetch: is normally used internally by the graph function to get data from RRDs.
    fetch will analyze the RRD and try to retrieve the data in the resolution requested.
- filename: the name of the RRD you want to fetch the data from.
- CF: the consolidation function that is applied to the data you want to fetch
- resolution: the interval you want the values to have (seconds per value).
- start: start of the time series. Negative numbers are relative to the current time
- end: the end of the time series in seconds since epoch.
*/
void RRDTool::fetch(quint64 resolution, quint64 start, quint64 end)
{
    MY_DEBUG;

    time.start();
    here = "first";
    stringArguments.clear();

    run();
}

/*USAGE:
rrdtool graph|graphv filename [option ...] [data definition ...] [data calculation ...]
    [variable definition ...] [graph element ...] [print element ...]
- graph: is used to present the data from an RRD to a human viewer.
http://oss.oetiker.ch/rrdtool/doc/rrdgraph.en.html
http://oss.oetiker.ch/rrdtool/doc/rrdgraph_data.en.html
http://oss.oetiker.ch/rrdtool/doc/rrdgraph_graph.en.html

Defaults are: 1 day ago until now. 400 pixels by 100 pixels, default legend is south

OPTIONS:
- filename: The name and path of the graph to generate

Time range
[-s|--start time] [-e|--end time] : the time series you would like to display
[-S|--step seconds] : By default, rrdtool graph calculates the width of one pixel in the time domain
    and tries to get data from an RRA with that resolution. With the step option you can alter this behavior.
    If you want rrdtool graph to get data at a one-hour resolution from the RRD, set step to 3'600
Labels
[-t|--title string] [-v|--vertical-label string] : labels
Size
[-w|--width pixels] [-h|--height pixels] : size
Limits
[-u|--upper-limit value] [-l|--lower-limit value] [-r|--rigid] : limits
X-Axis
[-x|--x-grid GTM:GST:MTM:MST:LTM:LST:LPR:LFM]
    ?TM : time in {SECOND, MINUTE, HOUR, DAY, WEEK, MONTH or YEAR}
    ?ST : steps between ?TM
    G?? : grid
    M?? : major grid
    L?? : labels
    LPR : precision. defines where each label will be placed. If it is zero, the label will be placed right under
        the corresponding line (useful for hours, dates etcetera). If you specify a number of seconds here the label
        is centered on this interval (useful for Monday, January etcetera).
    LFM : strftime format string
[-x|--x-grid none]
Y-Axis
[-y|--y-grid grid step:label factor] : Y-axis grid lines appear at each grid step interval.
    Labels are placed every label factor lines.
[-y|--y-grid none]
[-Y|--alt-y-grid] : Place the Y grid dynamically based on the graph's Y range
[-L|--units-length value] : How many digits should RRDtool assume the y-axis labels to be
Right Y Axis
[--right-axis scale:shift] [--right-axis-label label] : A second axis will be drawn to the right of the graph.
    It is tied to the left axis via the scale and shift parameters. You can also define a label for the right axis.
[--right-axis-format format-string] : use this option with the same %lf arguments
Legend
[--legend-position=(north|south|west|east)] : Place the legend at the given side of the graph.
[--legend-direction=(topdown|bottomup)] : Place the legend items in the given vertical order
Miscellaneous
[--zoom factor] : Zoom the graphics by the given amount. The factor must be > 0
[-a|--imgformat PNG|SVG|EPS|PDF]

We can have more rrd's from where to take data:
- each graph can be made from multiple rrd files
- each rrd file can have multiple data sources
- each data source can have multiple rras
DEF:<vname>=<rrdfile>:<ds-name>:<CF>[:step=<step>][:start=<time>][:end=<time>][:reduce=<CF>]
- vname: can be used throughout the rest of the script
- ds-name: datasource name from the rrd
- CF: consolidation function from the rra
- step: number of steps that create a point in the graph.
    This will override the step from the graph options and the one from the rra
- start: it will override the start from the graph options, if it exists.
    If the start options in the graph does not exist, the default period will be used
- end: the same as start
- reduce: If consolidation needs to be done, the CF of the RRA specified in the DEF itself will be used to reduce
    the data density. This behavior can be changed using :reduce=<CF>. This optional parameter specifies the CF to use
    during the data reduction phase.

COMMENT:text : Text is printed literally in the legend section of the graph
VRULE:time#color[:legend][:dashes[=on_s[,off_s[,on_s,off_s]...]][:dash-offset=offset]] : Draw a vertical line at time.
    Its color is composed from three hexadecimal numbers specifying the rgb color components (00 is off, FF is maximum)
    red, green and blue followed by an optional alpha. Optionally, a legend box and string is printed in the legend
    section. Dashed lines can be drawn using the dashes modifier.
HRULE:value#color[:legend][:dashes[=on_s[,off_s[,on_s,off_s]...]][:dash-offset=offset]] : the same as above
LINE[width]:value[#color][:[legend][:STACK]][:dashes[=on_s[,off_s[,on_s,off_s]...]][:dash-offset=offset]]
    Draw a line of the specified width onto the graph. width can be a floating point number.
    optional is the legend box and string which will be printed in the legend section if specified.
    The value can be generated by DEF, VDEF, and CDEF.
    If the optional STACK modifier is used, this line is stacked on top of the previous element which can be
        a LINE or an AREA.
    The dashes modifier enables dashed line style. Without any further options a symmetric dashed line with a
        segment length of 5 pixels will be drawn. The dash pattern can be changed if the dashes= parameter is
        followed by either one value or an even number (1, 2, 4, 6, ...) of positive values. Each value provides
        the length of alternate on_s and off_s portions of the stroke. The dash-offset parameter specifies an offset
        into the pattern at which the stroke begins.
AREA:value[#color][:[legend][:STACK]] : See LINE, however the area between the x-axis and the line will be filled.
TICK:vname#rrggbb[aa][:fraction[:legend]] : Plot a tick mark (a vertical line) for each value of vname that is
    non-zero and not *UNKNOWN*. The fraction argument specifies the length of the tick mark as a fraction of the y-axis.
    The TICK marks normally start at the lower edge of the graphing area. If the fraction is negative they start at the
    upper border of the graphing area.
*/
void RRDTool::graph()
{
    MY_DEBUG;

    time.start();
    here = "graph";
    stringArguments.clear();

    run();
}

/*USAGE:
rrdtool {update | updatev} filename [--template|-t ds-name[:ds-name]...] [--] N|timestamp:value[:value...]
    at-timestamp@value[:value...] [timestamp:value[:value...] ...]
- update: feeds new data values into an RRD
- filename: The name of the RRD you want to update.
- --: When using negative time values, options and data have to be separated by two dashes
- N|timestamp: N is current time, timestamp is defined in seconds since 1970-01-01
- value[:value...]: DS updates. The order of this list is the same as the order the data sources were defined in the RRA

also:
rrdtool - < file
file: "update filename time:value:value time:value:value..."
*/
void RRDTool::update()
{
    MY_DEBUG;

    time.start();
    here = "update";
    stringArguments.clear();

    message->sendUpdateInfo("Start writing rrd database",0);
    createUpdatePairs();

    QMap<quint64, QStringList>::const_iterator it = maprrd.constBegin();

#if defined (Q_OS_WIN32)
    int maxcmdlength = 8191;
    here = "update in stupid windows limit";
#else
    QProcess *proctmp;
    proctmp = new QProcess;
    proctmp->start(QString("getconf"), QStringList("ARG_MAX"));
    proctmp->waitForFinished();
    int maxcmdlength = proctmp->readAllStandardOutput().simplified().toInt() - 2048;
    proctmp->deleteLater();
#endif

    maxcmdlength -= process->systemEnvironment().join(" ").count();
    message->sendLog("Command length is " + QString::number(maxcmdlength));
    QString str;
    QStringList list, initialargs = stringArguments;
    int crtcmdlength = initialargs.join(" ").count();

    while (it != maprrd.constEnd()) {
        str = QString::number(it.key()) + ":" + it.value().join(":");
        crtcmdlength += str.length();
        if ( crtcmdlength > maxcmdlength ){
            message->sendLog("Command line length is too small. Can't do a single update.");
            break;
        }else{
            if ( crtcmdlength < maxcmdlength ){
                list << str;
                ++it;
            }else{
                stringArguments = initialargs;
                stringArguments  << list;
                run();
                process->waitForFinished();

                crtcmdlength = initialargs.join(" ").count();
                list.clear();
            }
        }
    }

    stringArguments = initialargs;
    stringArguments  << list;
    run();
}

/*USAGE:
rrdtool resize filename rra-num GROW|SHRINK rows
- resize: is used to modify the number of rows in an RRA
- rra-num: the RRA you want to alter. You can find the number using rrdtool info.
- GROW: used if you want to add extra rows to an RRA. The extra rows will be inserted as the rows that are oldest.
- SHRINK: used if you want to remove rows from an RRA. The rows that will be removed are the oldest rows.
- rows: the number of rows you want to add or remove.
*/
void RRDTool::resize()
{
    MY_DEBUG;

    time.start();
    here = "resize";
    stringArguments.clear();

    run();
}

/*USAGE:
rrdtool tune filename [--heartbeat|-h ds-name:heartbeat] [--minimum|-i ds-name:min] [--maximum|-a ds-name:max]
    [--data-source-type|-d ds-name:DST] [--data-source-rename|-r old-name:new-name]
- tune: allows you to alter some of the basic configuration values stored in the header area of a Round Robin Database (RRD)
*/
void RRDTool::tune()
{
    MY_DEBUG;

    time.start();
    here = "tune";
    stringArguments.clear();

    run();
}

/*USAGE:
rrdtool info filename.rrd
- The info function prints the header information from an RRD in a parsing friendly format.
*/
void RRDTool::info()
{
    MY_DEBUG;

    time.start();
    here = "info";
    stringArguments.clear();

    stringArguments << "info" << inforrd->getDatabasePath();

    run();
    stringExpectedError = "ERROR: '" + inforrd->getDatabasePath() + "' is not an RRD file";
    stringExpectedOutput = "filename = \"" + inforrd->getDatabasePath() + "\"";
}

/*USAGE:
rrdtool first filename [--rraindex number]
- first: returns the UNIX timestamp of the first data sample entered into the specified RRA of the RRD file.
- rraindex number: The index number of the RRA that is to be examined.
    If not specified, the index defaults to zero.
    RRA index numbers can be determined through rrdtool info
*/
void RRDTool::first()
{
    MY_DEBUG;

    time.start();
    here = "first";
    stringArguments.clear();

    stringArguments << "first" << inforrd->getDatabasePath();

    run();
}

/*USAGE:
rrdtool last filename
- last: The last function returns the UNIX timestamp of the most recent update of the RRD.
*/
void RRDTool::last()
{
    MY_DEBUG;

    time.start();
    here = "last";
    stringArguments.clear();

    stringArguments << "last" << inforrd->getDatabasePath();

    run();
}

/*USAGE:
rrdtool dump [--no-header|-n] filename.rrd > filename.xml
- dump: writes the contents of an RRD in human readable (?) XML format to a file or to stdout.
    This format can be read by rrdrestore.
- no-header: Unfortunately the rrdtool restore function from the 1.2 series can not handle the xml headers.
    With this option you can supress the creatinon of the xml headers.
*/
void RRDTool::dump()
{
    MY_DEBUG;

    time.start();
    here = "dump";
    stringArguments.clear();

    stringArguments << "dump" << inforrd->getDatabasePath();
    message->sendUpdateInfo("Start dump of database",100);

    run();
}

/*USAGE:
rrdtool restore filename.xml filename.rrd [--range-check|-r]
- restore: reads the XML representation of an RRD and converts it to the native RRD format.
- range-check: Make sure the values in the RRAs do not exceed the limits defined for the various data sources.
- force-overwrite|-f: Allows RRDtool to overwrite the destination RRD.
*/
void RRDTool::restore()
{
    MY_DEBUG;

    time.start();
    here = "restore";
    stringArguments.clear();

    run();
}

void RRDTool::stop()
{
    MY_DEBUG;

    if ( process->state() ){
        process->terminate();
        process->waitForFinished();
    }
    stringArguments.clear();
}

void RRDTool::createUpdatePairs()
{
    MY_DEBUG;

    QMap<quint64, double> map1, map2;
    QStringList dsnames, list;
    QRegExp regexp("\D");
    unsigned int nr = inforrd->getDatasourcesNumber();

    for (unsigned int i=0; i<nr-1; i++){
        dsnames << inforrd->getDatasource(i).name;
        //we don't parse the last datasource, because we don't have anything after that
        map1 = inforrd->getDatasource(i).values;
        QMap<quint64, double>::const_iterator it = map1.constBegin();
        while (it != map1.constEnd()) {
            list.clear();
            if ( i ){
                list = QString::QString().fill('U',i).split(regexp);
            }else{
                list << QString::number(it.value());
            }

            for (unsigned int j=i+1; j<nr; j++){
                map2 = inforrd->getDatasource(j).values;
                if ( map2.contains(it.key()) ){
                    list << QString::number(map2.value(it.key()));
                    //seems removing or replacing is waaaay to expensive: from 0.098s to 17.89s
                    //so, we only add new keys. see below
                }else{
                    list << "U";
                }
            }
            if ( !maprrd.contains(it.key()) ){
                maprrd.insert(it.key(), list);
            }
            ++it;
        }
    }
    //the last datasource is not parsed yet for new elements
    map2 = inforrd->getDatasource(nr-1).values;
    dsnames << inforrd->getDatasource(nr-1).name;
    QMap<quint64, double>::const_iterator it = map2.constBegin();
    while (it != map2.constEnd()) {
        list.clear();
        list = QString::QString().fill('U',nr-1).split(regexp);
        list << QString::number(it.value());
        if ( !maprrd.contains(it.key()) ){
            maprrd.insert(it.key(), list);
        }
        ++it;
    }

    stringArguments << "update" << inforrd->getDatabasePath() <<
                    "--template" << dsnames.join(":");
}

QString RRDTool::getExpectedError()
{
    MY_DEBUG;

    return stringExpectedError;
}

QString RRDTool::getExpectedOutput()
{
    MY_DEBUG;

    return stringExpectedOutput;
}

QString RRDTool::getError()
{
    MY_DEBUG;

    return stringError;
}

QString RRDTool::getOutput()
{
    MY_DEBUG;

    return stringOutput;
}

bool RRDTool::isRunning()
{
    MY_DEBUG;

    return process->state();
}

void RRDTool::waitforFinished()
{
    MY_DEBUG;

    process->waitForFinished();
}

RRDTool::~RRDTool()
{
    MY_DEBUG;
}

int RRDTool::returncode()
{
    MY_DEBUG;

    message->sendLog("Output for rrd " + here + " is:\n\t" + stringOutput.left(stringOutput.indexOf("\n")));
    message->sendLog("Expected output for rrd " + here + " is:\n\t" + stringExpectedOutput);
    message->sendLog("Error for rrd " + here + " is:\n\t" + stringError.simplified());
    message->sendLog("Expected error for rrd " + here + " is:\n\t" + stringExpectedError);

    if ( !stringError.isEmpty() && (stringError.startsWith(stringExpectedError)) ){
        return 1;
    }else{
        if ( stringOutput.startsWith(stringExpectedOutput) ){
            return 0;
        }else{
            return 2;
        }
    }
}
