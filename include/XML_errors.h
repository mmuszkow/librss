#ifndef __RSS_XML_ERRORS_H__
#define __RSS_XML_ERRORS_H__

/**
 * xmlErrorLevel:
 *
 * Indicates the level of an error
 */
typedef enum _xmlErrorLevel {
    XML_ERR_NONE = 0,
    XML_ERR_WARNING = 1,	/* A simple warning */
    XML_ERR_ERROR = 2,		/* A recoverable error */
    XML_ERR_FATAL = 3		/* A fatal error */
} xmlErrorLevel;

/**
 * xmlErrorDomain:
 *
 * Indicates where an error may have come from
 */
typedef enum _xmlErrorDomain {
    XML_FROM_NONE = 0,
    XML_FROM_PARSER,	/* The XML parser */
    XML_FROM_TREE,		/* The tree module */
    XML_FROM_NAMESPACE,	/* The XML Namespace module */
    XML_FROM_DTD,		/* The XML DTD validation with parser context*/
    XML_FROM_HTML,		/* The HTML parser */
    XML_FROM_MEMORY,	/* The memory allocator */
    XML_FROM_OUTPUT,	/* The serialization code */
    XML_FROM_IO,		/* The Input/Output stack */
    XML_FROM_FTP,		/* The FTP module */
    XML_FROM_HTTP,		/* The HTTP module */
    XML_FROM_XINCLUDE,	/* The XInclude processing */
    XML_FROM_XPATH,		/* The XPath module */
    XML_FROM_XPOINTER,	/* The XPointer module */
    XML_FROM_REGEXP,	/* The regular expressions module */
    XML_FROM_DATATYPE,	/* The W3C XML Schemas Datatype module */
    XML_FROM_SCHEMASP,	/* The W3C XML Schemas parser module */
    XML_FROM_SCHEMASV,	/* The W3C XML Schemas validation module */
    XML_FROM_RELAXNGP,	/* The Relax-NG parser module */
    XML_FROM_RELAXNGV,	/* The Relax-NG validator module */
    XML_FROM_CATALOG,	/* The Catalog module */
    XML_FROM_C14N,		/* The Canonicalization module */
    XML_FROM_XSLT,		/* The XSLT engine from libxslt */
    XML_FROM_VALID,		/* The XML DTD validation with valid context */
    XML_FROM_CHECK,		/* The error checking module */
    XML_FROM_WRITER,	/* The xmlwriter module */
    XML_FROM_MODULE,	/* The dynamically loaded module module*/
    XML_FROM_I18N,		/* The module handling character conversion */
    XML_FROM_SCHEMATRONV/* The Schematron validator module */
} xmlErrorDomain;

typedef struct _xmlError {
    int		domain;	/* What part of the library raised this error */
    int		code;	/* The error code, e.g. an xmlParserError */
    char*	message;/* human-readable informative error message */
    xmlErrorLevel level;/* how consequent is the error */
    char*	file;	/* the filename */
    int		line;	/* the line number if available */
    char*	str1;	/* extra string information */
    char*	str2;	/* extra string information */
    char*	str3;	/* extra string information */
    int		int1;	/* extra number information */
    int		int2;	/* column number of the error or 0 if N/A (todo: rename this field when we would break ABI) */
    void*	ctxt;   /* the parser context if available */
    void*	node;   /* the node in the tree */
} xmlError;

typedef xmlError *xmlErrorPtr;

typedef void (__cdecl *xmlStructuredErrorFunc) (void *userData, xmlErrorPtr error);

#endif
