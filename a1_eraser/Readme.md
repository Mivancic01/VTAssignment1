# Assignment 1: Eraser

* __Type__: Individual work
* __Purpose__: Understand the Eraser algorithm.
* __Task__: Implement the Eraser algorithm in C++.
* __Question Hour__: Oct 16, 2020 11am on [Discord](https://discord.gg/aeMjStg).
* __Deadline__: Oct 22 (23:59 Graz timezone), 2020.
(Commits after the deadline will be ignored)

##  Deliverable

To submit your solution, create a new branch `a1_submission` containing the version we should grade:

```bash
git checkout -b a1_submission
git push -u origin a1_submission
git checkout master
```

This will create a new branch callend `a1_submission` and change into that branch. After that, the new branch is pushed and you change back into the local master branch.

If you want to submit a new revision, just call
```bash
git checkout a1_submission
git merge master
git push origin
git checkout master
```

We will enable the test system in a few days - it will give you feed back, if your submission was successful.

## Dockerfile (Optional)
We included a Dockerfile installing all prerequisites.
After installing Docker, you simply need to execute the following commands:

```bash
sudo docker build -t eraser .
sudo docker run --rm -ti -v`pwd`:/a1_eraser --cap-add=SYS_PTRACE eraser
```

This will build a docker vm, mount the specified folder under /data and start a terminal.

We will use this Docker-Image to test your implementations, so we advise you to
use it as a reference system.


## Framework Layout

The framework directory structure looks as follows:
```
    a1_eraser/         ; The main project directory
    | cmake/             ; Directory with CMake modules
    | tests/             ; Provided public test files
    | inst/              ; Directory inst. targets
    | | tests/             ; Some programs you can instrument
    | ...                ; Source files for Eraser
```

We provide you with a directory already preconfigured to build eraser with stubs for all relevant functions you need to implement. You mostly need to change parts indicated with `\todo`. 

### Building the Project

The implementation also makes use of other open source and third-party libraries. To build the project you will need at least the following:

* CMake
* make
* a c\+\+14 compiler like (g\+\+ or clang)
* libunwind-dev
* libdw-dev
* liblzma-dev
* check
* binutils
* pkg-config
* python3

If some of these are not present on your system or are outdated, you should install them using your package manager. Any relevant Linux distribution will have a package providing these programs and libraries.

The pin library from Intel will be downloaded automatically when building code for the last two tasks.

Since the project uses `CMake`, it is important to have a basic understanding of how to properly work with your project structure. It is generally wise to put the compiled files in a separate directory and not build inside the source directory. Contaminating your source directory is the usual cause of binary file commits in git and might lead to us not being able to run your implementation. 

The following code shows how you should build your projects properly, assuming that you are currently in the `a1_eraser/` directory:

```bash
mkdir build
cd build
cmake ..
make -j
cd ..
```

This will create a build directory, populate it with automatically generated makefiles and build your whole project. You should not change the file `a1_eraser/CMakeLists.txt`. You might, however, need to change the file `a1_eraser/tests/CMakeLists.txt` to add custom tests, as we will ignore any changes you make to that directory.

You can execute the unit tests by doing:
```bash
cd build/tests
./test_basic
./test_mytest
```

## Overview of Helper Classes

### `class EraserLock`

`EraserLock` is a very simple wrapper for `std::mutex`, which tells the eraser algorithm that for some `std::mutex`, there was a call to `lock()` or `unlock()`.

### `template<class Type> class EraserObject`

`EraserObject<Type>` is a simple template class for any C++ object for which we want to track the accesses. We need some way to set and get values of the underlying object. This is done by overloading the assignment and function call operators:

```c++
// Object setter
void operator=(const Type& other);

// Object getter
const Type& operator()();
```

You do not need to implement this yourself. We provide an implementation which also gives you information about where the read or write was performed. The way this works is very interesting and might be a question during the interviews.

### `class Logger`

In order to issue warnings when you find a race condition of undefined behaviour, we provide you with a the class `Logger`. You do not need to edit this class, only use its public interface whenever you detect a concurency bug. The most relevant functions for you will be:

```c++
inline void uninitialisedRead(std::thread::id id, const void *object,
                              const void *ip, const std::string &objname);

inline void unprotectedRead(std::thread::id id, const void *object,
                            const void *ip, const std::string &objname);

inline void unprotectedWrite(std::thread::id id, const void *object,
                             const void *ip, const std::string &objname);
``` 

Accessing the currently registered `Logger` is done via the namespace variable `current_logger`. You can register a new `Logger` with `set_logger(Logger*)`. Deregistering is done with `clear_logger()`.

## Eraser

The `Eraser` class will be the base class for your implementations of the algorithm. It has a public interface which handles the basic actions which happen throughout the execution of the program.

```c++
virtual void lockCalled(std::thread::id id, const void *lock);
virtual void unlockCalled(std::thread::id id, const void *lock);
virtual void objectRead(std::thread::id id, void *object,
                        const void *ip, const std::string &objname) = 0;
virtual void objectWrite(std::thread::id id, void *object,
                         const void *ip, const std::string &objname) = 0;
```

It is important to synchronise any access to protected members of this class. For this purpose, you are provided with the member `std::mutex eraser_mutex_`, which you can use for synchronising accesses. You will have to add any protected member variables to this class, which you want to be available in its derived classes.

The best way to do the synchronisation of accesses to this class is through scoped locks like `std::unique_lock<T>`. Such a synchronisation might look like this:

```c++
{
  std::unique_lock<std::mutex> sync_lock(eraser_mutex_);
  // your code which accesses protected/private members
}
```

Since both the simple and advanced algorithms do the same for any call to `lock()` and `unlock()`, we recommend implementing this functionality in the base class to avoid code duplication.

Since we do not want to pass a pointer to the currently active `Eraser` object to each `EraserObject` and `EraserLock`, we have a namespace variable `current_algorithm`, which can be registered and deregistered similarly to `Logger`.

## Task 1: Simple Eraser [25 P.]

In this task, you will implement the basic Eraser algorithm introduced in the lecture. For this purpose, you will need to adapt the `SimpleEraser` class which is derived from class `Eraser`. The member functions which you need to implement are:

```c++
void objectRead(std::thread::id id, void *object, const void *ip, const std::string &objname);
void objectWrite(std::thread::id id, void *object, const void *ip, const std::string &objname);
```

These are called whenever an `EraserObject` is read or written. Note that, in contrast to the paper and lecture, you also need to report any read of an uninitialised object. In C++, this can only happen if the given object is a primitive type (`int`, `char`, `float`, ...) or a pointer. In all other cases, the default constructor is called, and without access to the machine code, you cannot detect whether an initialisation was performed or not (e.g. the difference between calloc() and malloc() in the constructor, initialisation of members, ...). We will assume that if a constructor is called, the whole object is initialised.

## Task 2: Advanced Eraser [40 P.]

Most of this task is the same as Task 1, with the only difference being that you now implement the advanced version of the Eraser algorithm inside the `AdvancedEraser`. In order to represent the object states
needed for advanced Eraser, you can make use of the `EraserInfo` structure which contains the enumeration `ObjectState`.

```c++
enum ObjectState
{
  VIRGIN, EXCLUSIVE, SHARED, SHARED_MODIFIED
};
```

You can add any additional functionality you need to this structure since you might need to perform some actions like set intersection very often.

## Task 3: Simple vs Advanced [15 P.]

The goal of this task is to highlight the differences between the two versions of Eraser. To do this, you will have to write your own testcases where the simple and advanced Eraser report different warnings. Complete the contents of file
`a1_eraser/tests/mytest.cpp` with the following testcases:

1. Complete test `simple_warn_write` where `SimpleEraser` reports an unprotected write and `AdvancedEraser` does not report any warnings.
2. Complete test `simple_warn_read` where `SimpleEraser` reports an unprotected read and `AdvancedEraser` does not report any warnings.
3. Complete test `simple_warn_lock` inside a protected region. `SimpleEraser` reports an unportected read, and `AdvancedEraser` does not report any warnings. You can overwrite the provided function `f()`.

The framework provides custom make targets for building tests for the first three tasks. After that you can just execute it like a normal binary.
```bash
make test_<test-name> && ./tests/test_<test-name>
```



## Task 4: Code Instrumentation [20 P.]

So far, we have used the helper classes `EraserLock` and `EraserObject` to inform our algorithm what is happening in the code. However, turning every object into an `EraserObject` is very costly and cumbersome, since you have to rewrite your whole codebase just for testing. The solution to this problem is to use instrumentation.

For this task, you will implement an instrumentation tool using our own `cemila` framework. You will instrument every read and write in the assembler code. What this means is that whenever something like a `mov` is done in the assembler code, you will insert a call to a function of your choice. This might look like the following:

```assembly
                   ; -> ; loading of registers
                   ; -> call beforeWrite
    mov [var], rbx ; -> mov [var], rbx
                   ; -> ; loading of registers
```

In this case, a call to the function `beforeWrite` is inserted before the relevant instruction.

In addition to reads and writes, you will also instrument calls to `std::muted::lock()` and `std::mutex::unlock()`.

We already provide you with a framework that inserts the function calls.
In the file `inst/cemila.cpp` we provide stubs for instrumenting reads, writes and the calls to mutex lock and unlock.

```c++
void report_load(void *addr, size_t size, void *ip);
void report_store(void *addr, size_t size, void *ip);
int pmutex_lock(pthread_mutex_t* mux);
int pmutex_unlock(pthread_mutex_t* mux);
```

Your task is to implement these stubs using the code from the advanced eraser code. The `Eraser` class can be accessed via the namespace variable `eraser::current_algorithm` where you need to call the appropriate methods.

In the instrumented code, we want to track access not on the object level, but on the byte level. 
The functions `report_load` and `report_store` provide you with the accessed address as well as the number of accessed bytes.
You have to convert this to report read and write access for each byte individually.
Note the `objname` parameter in the methods of `Eraser`, for this task it should always be an empty string. This tells the logger to report memory addresses instead of object names.

To test the code instrumentation and of the byte precise tracking you have to write your own test case. This test case should not produce any errors when only the address is reported, but it must produce an error if your byte precise implementation is used. It does not matter which of the errors you produce. Implement this test case in the file `inst/tests/my_instr_test.cpp`.

Building the test cases and instrumenting them is implemented using cmake.
If you build the project as described above you should find an `inst_test.elf` file for each of the tests in `build/inst`. These files are instrumented with the code in `cemila.cpp`. You can execute them and the eraser warnings are printed to `stderr`.
