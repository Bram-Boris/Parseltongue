#ifndef FILE_FILE_FORMAT_HPP
#define FILE_FILE_FORMAT_HPP

template<typename T>
class FileFormat {
public:
    template<typename F>
    F request() {
        F out {};
        stream.read(&f, sizeof(F));
    }
    template<typename F, int I>
    F request_array() {

    }
private:

};

#endif // FILE_FILE_FORMAT_HPP