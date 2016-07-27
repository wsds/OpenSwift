#include "Swift.h"

bool isSourceFile1(char *fileName) {
    bool result = false;
    char *i = fileName;

    while (*i) {
        i++;
    }

    if (*(i - 1) == 's' && *(i - 2) == 's' && *(i - 3) == '.') {
        result = true;
    }
    return result;
}

DMJSON *lookupFolder1(char *path) {
    DMJSON *result = newDMJSON();

    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(path)) == NULL) {
        return NULL;
    }
    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
        } else {
            if (isSourceFile1(entry->d_name)) {
                DMString *fileName = newDMString(getLength(entry->d_name) + 1);
                fileName->copy(entry->d_name);
                result->set(-1, (MemorySpace *) fileName, NOT_REPLACE);
            }
        }
    }
    closedir(dp);

    return result;
}

DMString *readFile1(char *fileName) {

    int fd = open(fileName, O_CREAT | O_RDWR, 0777);

    unsigned int file_length = -1;
    struct stat fileStat;
    if (-1 == fstat(fd, &fileStat)) {
        file_length = 0;
        return NULL;
    } else {
        file_length = fileStat.st_size;
    }

    DMString *result = newDMString(file_length);
    read(fd, result->char_string, file_length);
    result->used_length = file_length;
    return result;

}

void Swift::importCode(int contextID, char *path) {
    DMJSON *files = lookupFolder1(path);
    chdir(path);
    int i = 1;
    i++;
    DMString *code;
    for (int i = 0; i < files->dm_list->length; i++) {
        DMString *fileName = (DMString *) files->get(-1, NOT_DEL);
        code = readFile1(fileName->char_string);
        this->inputCode(contextID, code);
    }
    chdir("./");
}

void Swift::inputCode(int contextID, DMString *code) {
    Parser *parser = Parser::getInstance();
    DMJSON *code_block = parser->parseCode(code->char_string, code->used_length);
    DMString *code_string = stringifyCode(code_block);
    std::cout << code_string->char_string;
    std::cout << "Parse DONE ####\n";

    Excutor *excutor = Excutor::getInstance();
    MemorySpace *contextSpace = this->context_pool->get(contextID, NOT_DEL);
    Context *context = NULL;
    if (contextSpace != NULL) {
        context = (Context *) contextSpace->pointer;
    } else {
        context = newDMContext();
        importModules(context->globle);
        this->context_pool->set(contextID, context->self, NOT_REPLACE);
    }

    excutor->excute(code_block, context);

    std::cout << "DONE ####";

    int i = 0;
    i++;
}

Swift::Swift() {
    this->is_initialized = false;
    this->initialize();
}

Swift *Swift::instance = NULL;

Swift *Swift::getInstance() {
    if (instance == NULL) {
        instance = new Swift();
    }
    return instance;
}

bool Swift::initialize() {
    if (this->is_initialized == true) {
        return true;
    }

    this->context_pool = newDMJSON();
    this->is_initialized = true;

    return true;
}

