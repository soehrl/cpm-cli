# CMake Project Manager
Creating CMake projects from scratch requires a lot of boilerplate code to be written before you can start writing the actual code.
Adding dependencies requires even more work to be done.
`cpm-cli` is a command line interface that streamlines workflow of cmake projects similar to cargo/npm.
Instead of writing your CMakeLists.txt yourself `cpm-cli` provide a set of useful commands to help you:

- `cpm create [project_name]` creates a new folder with a ready-to-go cmake configuration.
  It adds a default executable target containing a "Hello World" main function.
  It also sets up the project for using the package manager [CPM](https://github.com/cpm-cmake/CPM.cmake), so adding dependencies becomes very easy.
- `cpm configure` will configure your cmake project.
- `cpm build` will build your cmake project. If it has not been configured yet, it will do so before.
- `cpm add executable/library [name]` will add a executable or library target to your cmake project.

The best part is: `cpm-cli` does not force itself onto anyone.
If you use it for your project other maintainers or users can happily work on or use the codebase with the regular cmake commands.

## Example
The following shows an example of creating and building a project with `cpm-cli`.
```
> cpm create awesome_project
> cd awespme_project
> cpm build
[info] Project has not been configured yet
-- The CXX compiler identification is AppleClang 14.0.0.14000029
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /Library/Developer/CommandLineTools/usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: ./build
[ 50%] Building CXX object awesome_project/CMakeFiles/awesome_project.dir/src/awesome_project.cpp.o
[100%] Linking CXX executable awesome_project
[100%] Built target awesome_project
> ./build/awesome_project/awesome_project
Hello World!
```

## Project structure
Currently `cpm-cli` will produce/assume a certain structure for CMake projects.
In the future a configuration file may alter this structure.
The following shows the folder structure for a CMake project containing a library and a executable target.

```
- project_name/
+-- CMakeLists.txt
+-- cmake
| +-- CPM.cmake
|
+- library_target/
| +-- CMakeLists.txt
| +-- include/project_name/library_target/
| | +-- foo.hpp
| +-- src/
|   +-- foo.cpp
|
+-- executable_target/
  +-- CMakeLists.txt
  +-- src/
    +-- executable_target.cpp
```

## Roadmap
- **`run` command to execute build targets.**
  This would change the `./build/awesome_project/awesome_project` from the example above to the probably more user friendly `cpm run awesome_project`.
- **Support for test and benchmarks.**
- **Proper install scripts.**
  The project should have properly set-up install targets out of the box.
- **Commands for managing external packages.**
  Currently the created project is set-up for using [CPM](https://github.com/cpm-cmake/CPM.cmake), however, commands like `cpm add package [package_name]` would be nice.
- **Create package repository for commonly used packages.**
- **Way more options and a confugration file.**
  
