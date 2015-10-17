#ifndef SCRIPTPARSER_H
#define SCRIPTPARSER_H

#include <QString>

typedef struct _object PyObject;

class ScriptParser
{
public:
    static ScriptParser *instance();
    static void parse(QString filename);

    static PyObject *module(void);

private:
    ScriptParser();
    static ScriptParser *_instance;
    static PyObject *_module;
};

#endif // SCRIPTPARSER_H
