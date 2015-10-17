#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <stdarg.h>
#include "Python.h"
#include "ScriptParser.h"

ScriptParser *ScriptParser::_instance = 0;
PyObject *ScriptParser::_module = 0;

ScriptParser *ScriptParser::instance()
{
    if (0 == _instance) {
        return new ScriptParser;
    }

    return _instance;
}

void ScriptParser::parse(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << QObject::tr("File [%1] doesn't exist.\n").arg(filename);
        return;
    }

    QTextStream stream(&file);
    QString context;

    while (!stream.atEnd()) {
        context.append(stream.readLine());
        context.append("\n");
    }

    PyRun_SimpleString(qPrintable(context));
}

PyObject *ScriptParser::module()
{
    return _module;
}

ScriptParser::ScriptParser()
{
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./scripts')");
    _module = PyImport_Import(PyString_FromString("global"));
}
