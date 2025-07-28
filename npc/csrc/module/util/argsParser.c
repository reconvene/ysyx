#include <stdio.h>
#include <getopt.h>
#include <stdint.h>
#include <npc.h>

static const struct option argsTable[] {
    {"help",    no_argument,        NULL, 'h'},
    {"batch",   no_argument,        NULL, 'b'},
    {"image",   required_argument,  NULL, 'i'},
    {"elf",     required_argument,  NULL, 'e'},
    {"diff",    required_argument,  NULL, 'd'}
};

static const char *argsDescription[] = {
    "list all arguments' usage",
    "set NPC in the batch mode",
    "load image from the .bin file",
    "the elf file will be used to function tracer",
    "the .so file will be used to difftest"
};

static const size_t argsNum=sizeof(argsTable)/sizeof(struct option);

static void argsHelp() {
    for (size_t i=0;i<argsNum;i++) printf("%s %s\n",argsTable[i].name,argsDescription[i]);
}

void loadImage(const char* filename);
void init_ELFParser(const char *elfFileName);
void init_difftest(char *fileName, word_t imageSize);

static char *imageFile=NULL;
static char *elfFile=NULL;
static char *diffFile=NULL;

void argsParser(int argc, char *argv[]) {
    int opt=0;
    while ((opt = getopt_long(argc,argv, "-hbi:e:",argsTable,NULL)) != -1) {
        switch (opt) {
            case 'h': argsHelp(); break;
            case 'b': batchMode=1; break;
            case 'i': imageFile=optarg; break;
            case 'e': elfFile=optarg; break;
            case 'd': diffFile=optarg; break;
            default: printf("unknown option %c\n",opt); break;
        }
    }

    loadImage(imageFile);
    IFDEF(CONFIG_FTRACE,init_ELFParser(elfFile));
    IFDEF(CONFIG_DIFFTEST,init_difftest(diffFile,imageSize));
}