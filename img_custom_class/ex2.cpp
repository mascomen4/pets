//
// Created by pi on 12/28/22.
//

#include <iostream>
#include <iomanip>

// 1. allocate function should handle the case with insufficient memory
// 2. if you define a ctor and dtor for a class you need to define operator= and copy ctor. <- improvement
// 3. also add the possibility to define a new handler for a class
// 4. we would want also to add support of load/save functionality

// 5. Как могут быть использованы умные указатели? Можно использовать умный указатель для хранения data.
// В таком случае, компилятор будет беспокоиться о правильности разрушения объекта а не мы.

// --- NewHandlerSupport ---
// работаем с динамическими данными, поэтому было бы удобно завести класс поддерживающий случаи когда памяти в heap
// нет. Можно через try-catch, try-catch заставляет переписывать классы, что усложнет их поддержку в дальнейшем
// Через класс NewHandlerSupport мы можем всего-лишь наследовать его, а весь остальной код не требует изменений.

template<class X>
class NewHandlerSupport{
public:
    static std::new_handler set_new_handler(std::new_handler p); // function should exist even if the object don't
    static void* operator new(size_t size); // allocator should exist before the object is constructed
private:
    static std::new_handler current_handler;
};

template<class X>
std::new_handler NewHandlerSupport<X>::set_new_handler(std::new_handler p){
    std::new_handler old_handler = current_handler;
    current_handler = p;
    return old_handler;
};

template<class X>
std::new_handler NewHandlerSupport<X>::current_handler; // don't forget to initialize the default handler to nullptr


template<class X>
void* NewHandlerSupport<X>::operator new(size_t size) {
    std::new_handler global_handler = std::set_new_handler(current_handler);
    void * memory;
    try{
        memory = ::operator new(size); // calling global operator new
    }
    catch(std::bad_alloc&){ // ::new tries to allocate some memory calling new_handler. if it eventually fails, we throw
                            // but getting back to the prev handler.
        std::cout << "Failed to allocate a memory, handler failed to handle the case as well. Aborting..." << std::endl;
        std::set_new_handler(global_handler);
        throw;
    }
    std::set_new_handler(global_handler);
    return memory;
}

// ---- Class Image ----

struct Box;

template<typename PixelType>
class Image : public NewHandlerSupport<Image<PixelType>> { // inheriting NewHandlerSupport gives support of new_handler function, and custom operator new.
public:
    Image(int width, int height, int channels): _width(width), _height(height), _channels(channels) { // even though for fundamental types no difference between
                                                                                                      // 2 or 1 call. It's a good habit to use when possible member initializer list
        allocate(width, height, channels);
    }

    ~Image() {
        delete[] data; // data stores amount of memory it has allocated just before the start of array.
    }

    // if you define a ctor and dtor for a class you need to define operator= and copy ctor.
    Image( const Image& image ): NewHandlerSupport<Image>(image), // base classes need initialization as well
            _width(image._width), _height(image._height), _channels(image._channels){ // during copy construction the elements are not copied by default. It's your job now.
        allocate(_width, _height, _channels);
        for (int col=0; col<_width; ++col){
            for (int row=0; row<_height; ++row){
                for (int channel=0; channel<_channels; ++channel){
                    data[ channel*(_width*_height) + row*(_width) +  col ] = image.data[ channel*(_width*_height) + row*(_width) + col ];
                    //        ^                       ^               ^
                    //        |                       |               |
                    // (image switching)       (row switching) (column switching)
                }
            }
        }
    }

    // if you define a ctor and dtor for a class you need to define operator= and copy ctor.
    Image & operator =(const Image& image){
        if ( this != &image ){ // check for self-assignment
            NewHandlerSupport<Image>::operator=(image); // base members need also to be rewritten

            // if the dimensions differ, allocate memory again and delete previous pointer
            if (image._width != _width || image._height != _height || image._channels != _channels){
                _width = image._width;
                _height = image._height;
                _channels = image._channels;
                delete[] data; data = nullptr;
                allocate(_width, _height, _channels);
            }

            // copy els
            for (int col=0; col<_width; ++col){
                for (int row=0; row<_height; ++row){
                    for (int channel=0; channel<_channels; ++channel){
                        data[ channel*(_width*_height) + row*(_width) + col ] = image.data[ channel*(_width*_height) + row*(_width) + col ];
                    }
                }
            }
        }
        return *this;
    }

    void SetData(PixelType * p){ // we need SetData to use it in the main function.
        data = p;
    }

    PixelType * GetData() {
        return data;
    }

    void Print(){
        std::cout << std::string(50, '-') << '\n' << std::endl;
        for (int channel=0; channel<_channels; ++channel) {
            for (int row = 0; row < _width; ++row) {
                for (int col = 0; col < _height; ++col) {
                    std::cout << std::setw(4) << data[channel * (_width * _height) + row * (_width) + col] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << "\n" << std::endl;
        }
    }

    template <typename T>
    friend void threshold(const Image<T>& image, Image<T>& outImage, T thresholdValue);

    template<typename T>
    friend Image<T> cropImage(const Image<T>& image, Box cropBox);

private:
    void allocate(int width, int height, int channels) {
        data = new PixelType[width * height * channels]; // if we fail to allocate here, the handler will try to allocate some new memory
    }

private:
    PixelType *data = nullptr;
    int _width;
    int _height;
    int _channels;
};

// --- Handler ----

int * p; // allocate some memory at a program startup, and delete it in case of insufficient memory
void out_of_memory_handle(){
    delete[] p;
}

struct Box {
    int xmin,xmax,ymin,ymax;
};

// Возвращает обрезанное изображение
template<typename PixelType>
Image<PixelType> cropImage(const Image<PixelType>& image, Box cropBox) {
    Image<PixelType> outImage(cropBox.xmax - cropBox.xmin + 1, cropBox.ymax - cropBox.ymin + 1, image._channels); // (a - b) is [a,b) interval, we want [a,b], so we add +1
    auto * data = new PixelType[ (cropBox.xmax - cropBox.xmin + 1) * (cropBox.ymax - cropBox.ymin + 1) * image._channels ]; // create a new data object, which is of the size of a croppedBox

    // copy element-by-element from initial Image to croppedImage
    for (int col=cropBox.xmin; col<=cropBox.xmax; ++col){
        for (int row=cropBox.ymin; row<=cropBox.ymax; ++row){
            for (int channel=0; channel<image._channels; ++channel){
               data[ channel*(outImage._width*outImage._height) + (row-cropBox.ymin)*(outImage._width) + (col-cropBox.xmin) ] = // to make it fill from 0 to (xmax-xmin) we substract xmin and ymin from col and row respectively
                       image.data[ channel*(image._width*image._height) + row*(image._width) + col ];
            }
        }
    }

    outImage.data = data;
    return outImage;
}

// Если значение пикселя меньше чем thresholdValue то ставим значение 0, иначе 1
template<typename PixelType>
void threshold(const Image<PixelType>& image, Image<PixelType>& outImage, PixelType thresholdValue) {
    if (&image != &outImage){
        if (image._width != outImage._width || image._height != outImage._height || image._channels != outImage._channels){
            std::cerr << "Image dimensions do not match! Exiting..." << std::endl;
            throw;
        }

        // update values iterating element-by-element
        for (int col=0; col<image._width; ++col){
            for (int row=0; row<image._height; ++row){
                for (int channel=0; channel<image._channels; ++channel){
                    PixelType & val = image.data[ channel*(image._width*image._height) + row*(image._width) + col ];
                    PixelType & destination_val = outImage.data[ channel*(image._width*image._height) + row*(image._width) + col ];
                    if (val < thresholdValue){
                        destination_val = 0;
                    }
                    else{
                        destination_val = 1;
                    }
                }
            }
        }
    }
}

int main(){
    // allocate some memory at program start so that handler can take from it.
    int allocation_size = 8 * 1024 * 1024; // allocate 8MB of memory in the heap.
    p = new int[allocation_size];

    Image<int>::set_new_handler(out_of_memory_handle); // set the handler

    int width = 10, height = 10, channels = 3;
    Image<int> img(width, height, channels);

    // fill image with i+j
    int * data = new int[width * height * channels];
    for (int col=0; col<width; ++col){
        for (int row=0; row<height; ++row){
            for (int channel=0; channel<channels; ++channel){
                data[ channel*(width*height) + row*(width) + col ] = row + col;
            }
        }
    }

    // ---- A bit of tests ---

    img.SetData(data);
    img.Print();

    // I could set up automatic tests, but it would take more time :0
    Box cropBox{1,3,1,3}; // Aggregate class -> we can use std::initializer_list
    Image<int> croppedImage = cropImage(img, cropBox);
    croppedImage.Print();

    Image<int> img2 = croppedImage;
    threshold(croppedImage, img2, 0);
    img2.Print();

}