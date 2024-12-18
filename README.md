# xsde - XML Schema to C++ data binding compiler for mobile and embedded systems

XSD/e is an open-source, dependency-free XML Schema to C++ compiler for
mobile, embedded, and light-weight applications. It provides XML parsing,
serialization, XML Schema validation and XML data binding while maintaining a
small footprint and portability.

For further information, including licensing conditions, documentation, and
binary packages, refer to the [XSD/e project
page](https://codesynthesis.com/products/xsde/).

NOTE: the steps described below are more appropriate for the development of
XSD/e as opposed to consumption. In case you just want to use XSD/e:

* If you want to use a binary package, see the [XSD/e download
  page](https://codesynthesis.com/products/xsde/download.xhtml).

* If you want to build XSD/e from source and use the result from a project
  that uses a build system other than `build2`, then see [Installing XSD/e with
  `build2`](https://codesynthesis.com/products/xsde/doc/install-build2.xhtml)
  for the step-by-step instructions.

* If you want to use XSD/e from a project that uses `build2` as the build
  system, then see the accompanying `PACKAGE-README.md` file.

The development setup for XSD/e uses two build configurations, for example:

```
git clone .../xsde.git
cd xsde

bdep init --empty

bdep config create @host ../xsde-host --type host cc config.cxx=g++
bdep config create @target ../xsde-target cc config.cxx=g++

bdep init @host -d xsde
bdep init @target -d libxsde -d xsde-tests -d xsde-examples

```

To generate the documentation in the `.ps` and `.pdf` formats, the `html2ps`
and `ps2pdf14` programs are required (the latter is from `ghostscript`). A
warning is issued in the development mode if these programs are not available.

To test installation of the XSD/e compiler, create a separate target
configuration (it will automatically resolve any build-time dependencies from
`host`):

```
bdep config create @install ../xsde-install --type target cc config.cxx=g++ \
  config.install.root=/tmp/install

bdep init @install -d xsde

b install: ../xsde-install/xsde/
```

The checked out `libxsde/xsde/cxx/version.hxx` will be overwritten during the
in-source build but these changes must be ignored. To do this automatically,
run:

```
git update-index --assume-unchanged libxsde/xsde/cxx/version.hxx
```
