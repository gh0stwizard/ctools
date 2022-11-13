#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include "perrorf.h"

#define PROGNAME colorizer

/* to print app version */
#define _STR(x) #x
#define STR(x) _STR(x)

#define remove_crlf(line, read) do { \
    if ((read) >= 2 && (memcmp((line) + (read) - 2, "\r\n", 2)) == 0) \
        (line)[(read)-2] = '\0'; \
    else if ((read) >= 1 && (line)[(read)-1] == '\n') \
        (line)[(read)-1] = '\0'; \
} while (0)


typedef enum {
    ANSI_MODE_RESET     = 0,
    ANSI_MODE_BOLD      = 1,
    ANSI_MODE_DIM       = 1<<1,
    ANSI_MODE_ITALIC    = 1<<2,
    ANSI_MODE_UNDERLINE = 1<<3,
    ANSI_MODE_BLINK     = 1<<4,
    ANSI_MODE_REVERSE   = 1<<5,
    ANSI_MODE_HIDDEN    = 1<<6,
    ANSI_MODE_STRIKE    = 1<<7
} ansi_mode;


typedef struct app_options_s {
    int modes;
    char *fg;
    char *bg;
} app_options_t;


typedef struct ansi_color_s {
    char *name;
    size_t name_length;
    int fg;
    int bg;
} ansi_color_t;

typedef struct ansi_mode_s {
    char *name;
    size_t name_length;
    int set;
    int unset;
} ansi_mode_t;


static ansi_color_t colors[] = {
    { "black",   6, 30, 40 },
    { "red",     4, 31, 41 },
    { "green",   6, 32, 42 },
    { "yellow",  7, 33, 43 },
    { "blue",    5, 34, 44 },
    { "magenta", 8, 35, 45 },
    { "cyan",    5, 36, 46 },
    { "white",   6, 37, 47 },
    { "default", 8, 39, 49 },
    { "reset",   6, 0,  0  },
    { NULL }
};


static ansi_mode_t modes[] = {
    { "reset",     6,  0, 0  },
    { "bold",      5,  1, 22 },
    { "dim",       4,  2, 22 },
    { "italic",    7,  3, 23 },
    { "underline", 10, 4, 24 },
    { "blink",     6,  5, 25 },
    { "reverse",   8,  7, 27 },
    { "hidden",    7,  8, 28 },
    { "strike",    7,  9, 29 },
    { NULL }
};

static void
print_usage()
{
    printf("Usage: %s [OPTIONS] [FILE1, FILE2, ...]\n"
           "Options:\n", STR(PROGNAME));
#define p(o, d) printf("  %-28s %s\n", (o), (d))
    p("-h, --help",             "print this help");
    p("-v, --version",          "print version");
    p("-R, --reverse",          "enable reverse mode");
    p("-B, --bold",             "enable bold mode");
    p("-D, --dim",              "enable dim/faint mode");
    p("-I, --italic",           "enable italic mode");
    p("-U, --underline",        "enable underline mode");
    p("-L, --blinking",         "enable blinking mode");
    p("-H, --hidden",           "enable hidden/invisible mode");
    p("-S, --strike",           "enable strikethrough mode");
    p("-f, --fg <COLOR>",       "foreground color");
    p("-b, --bg <COLOR>",       "background color");
    printf("\nColors:\nblack, red, green, yellow, blue, magenta, cyan, white, default, reset\n");
#undef p
}


static void
print_version()
{
    const char futures[] = {
        ""
    };
    printf("%s v%s%s\n", STR(PROGNAME), STR(APP_VERSION), futures);
}


static void
init_app_options(app_options_t *o)
{
   o->modes = 0;
   o->fg = NULL;
   o->bg = NULL;
}


static int
parse_args(int argc, char *argv[], app_options_t *o)
{
    int r;
    static struct option opts[] = {
        { "help",       no_argument,        0, 'h' },
        { "version",    no_argument,        0, 'v' },
        { "reverse",    no_argument,        0, 'R' },
        { "bold",       no_argument,        0, 'B' },
        { "dim",        no_argument,        0, 'D' },
        { "italic",     no_argument,        0, 'I' },
        { "underline",  no_argument,        0, 'U' },
        { "blinking",   no_argument,        0, 'L' },
        { "hidden",     no_argument,        0, 'H' },
        { "strike",     no_argument,        0, 'S' },
        { "fg",         required_argument,  0, 'f' },
        { "bg",         required_argument,  0, 'b' },
        { 0, 0, 0, 0 }
    };

    /* set default app options */
    init_app_options(o);

    while (1) {
        int idx = 0;
        r = getopt_long(argc, argv, "hvf:b:m:BDIURLHS", opts, &idx);
        if (r == -1) break;
        switch (r) {
        case 0:     break;
        case 'f':   o->fg = optarg; break;
        case 'b':   o->bg = optarg; break;
        case 'B':   o->modes |= ANSI_MODE_BOLD; break;
        case 'D':   o->modes |= ANSI_MODE_DIM; break;
        case 'I':   o->modes |= ANSI_MODE_ITALIC; break;
        case 'U':   o->modes |= ANSI_MODE_UNDERLINE; break;
        case 'R':   o->modes |= ANSI_MODE_REVERSE; break;
        case 'L':   o->modes |= ANSI_MODE_BLINK; break;
        case 'H':   o->modes |= ANSI_MODE_HIDDEN; break;
        case 'S':   o->modes |= ANSI_MODE_STRIKE; break;
        case 'h':   print_usage(); return 0;
        case 'v':   print_version(); return 0;
        default:    print_usage(); return 1;
        }
    }

    return -1;
}


static char *
get_color_sequence(char *modename, char *fgname, char *bgname)
{
    short mode = 0, fg = 0, bg = 0;

    if (modename) {
       for (ansi_mode_t *m = modes; m != NULL; m++) {
            if (strncmp(m->name, modename, m->name_length) == 0) {
                mode = m->set;
                break;
            }
        }
        if (strlen(modename) > 0 && mode == 0)
            fprintf(stderr, "mode %s not found\n", modename);
    }

    if (fgname) {
        for (ansi_color_t *c = colors; c != NULL; c++) {
            if (strncmp(c->name, fgname, c->name_length) == 0) {
                fg = c->fg;
                break;
            }
        }
        if (strlen(fgname) > 0 && fg == 0)
            fprintf(stderr, "fg color %s not found\n", fgname);
    }

    if (bgname) {
        for (ansi_color_t *c = colors; c != NULL; c++) {
            if (strncmp(c->name, bgname, c->name_length) == 0) {
                bg = c->bg;
                break;
            }
        }
        if (strlen(bgname) > 0 && bg == 0)
            fprintf(stderr, "bg color %s not found\n", bgname);
    }

    char *seq = malloc(sizeof(*seq) * 32);
    assert(seq != NULL);

    if (mode > 0) {
        if (fg > 0 && bg > 0)
            snprintf(seq, sizeof(*seq) * 32, "\33[%i;%i;%im", mode, fg, bg);
        else if (fg > 0 || bg > 0)
            snprintf(seq, sizeof(*seq) * 32, "\33[%i;%im", mode, fg + bg);
        else
            snprintf(seq, sizeof(*seq) * 32, "\33[%im", mode);
    }
    else {
        if (fg > 0 && bg > 0)
            snprintf(seq, sizeof(*seq) * 32, "\33[%i;%im", fg, bg);
        else if (fg > 0 || bg > 0)
            snprintf(seq, sizeof(*seq) * 32, "\33[%im", fg + bg);
        else
            seq[0] = '\0';
    }

//    fprintf(stderr, "%i;%i;%i => %s\n", mode, fg, bg, seq+1);

    return seq;
}


static int
colorize(const char *path, app_options_t *o)
{
    FILE *fh;
    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    char *color = NULL;


    if (!strcmp("-", path))
        fh = stdin;
    else
        fh = fopen(path, "r");

    if (!fh) {
        perrorf("parse: fopen %s", path);
        return 2;
    }

    char *last_mode = NULL;
    char mode_line[64] = {0};
    char *mode = mode_line;
    while (o->modes) {
        if (o->modes & ANSI_MODE_BOLD) {
            color = get_color_sequence("bold", NULL, NULL);
            o->modes &= ~ANSI_MODE_BOLD;
            last_mode = "bold";
        }
        else if (o->modes & ANSI_MODE_DIM) {
            color = get_color_sequence("dim", NULL, NULL);
            o->modes &= ~ANSI_MODE_DIM;
            last_mode = "dim";
        }
        else if (o->modes & ANSI_MODE_ITALIC) {
            color = get_color_sequence("italic", NULL, NULL);
            o->modes &= ~ANSI_MODE_ITALIC;
            last_mode = "italic";
        }
        else if (o->modes & ANSI_MODE_UNDERLINE) {
            color = get_color_sequence("underline", NULL, NULL);
            o->modes &= ~ANSI_MODE_UNDERLINE;
            last_mode = "underline";
        }
        else if (o->modes & ANSI_MODE_BLINK) {
            color = get_color_sequence("blink", NULL, NULL);
            o->modes &= ~ANSI_MODE_BLINK;
            last_mode = "blink";
        }
        else if (o->modes & ANSI_MODE_REVERSE) {
            color = get_color_sequence("reverse", NULL, NULL);
            o->modes &= ~ANSI_MODE_REVERSE;
            last_mode = "reverse";
        }
        else if (o->modes & ANSI_MODE_HIDDEN) {
            color = get_color_sequence("hidden", NULL, NULL);
            o->modes &= ~ANSI_MODE_HIDDEN;
            last_mode = "hidden";
        }
        else if (o->modes & ANSI_MODE_STRIKE) {
            color = get_color_sequence("strike", NULL, NULL);
            o->modes &= ~ANSI_MODE_STRIKE;
            last_mode = "strike";
        }
        else {
            fprintf(stderr, "unknown modes: %i\n", o->modes);
            break;
        }
        if (color) {
            size_t len = strlen(color);
            memcpy(mode, color, len);
            mode += len;
            free(color);
            color = NULL;
        }
    }

    // last_mode is not neccessary, could be NULL as well
    color = get_color_sequence(last_mode, o->fg, o->bg);
    while ((read = getline(&line, &len, fh)) != EOF) {
        remove_crlf(line,read);
        printf("%s%s%s\33[0m\n", mode_line, color, line);
    }

    fclose(fh);

    return -1;
}


extern int
main(int argc, char *argv[])
{
    app_options_t o;
    int r = parse_args(argc, argv, &o);
    if (r != -1) return r;

    if (optind == argc || (argc - optind == 1 && strcmp(argv[optind], "-") == 0))
        return colorize("-", &o);

    for (int i = optind; i < argc; i++) {
        r = colorize(argv[i], &o);
        if (r != -1) return r;
    }

    return 0;
}
