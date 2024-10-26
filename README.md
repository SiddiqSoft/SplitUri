SplitUri : Parse Uri Utility
-------------------------------------------
<!-- badges -->
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.SplitUri?branchName=main)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=14&branchName=main)
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.SplitUri?branchName=develop)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=14&branchName=develop)
![](https://img.shields.io/nuget/v/SiddiqSoft.SplitUri)
![](https://img.shields.io/github/v/tag/SiddiqSoft/SplitUri)
![](https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/14)
<!--![](https://img.shields.io/azure-devops/coverage/siddiqsoft/siddiqsoft/14)-->
<!-- end badges -->

# Objective

Parse the Uri for HTTP resources with support for `std::format` and `nlohmann.json` serializers.

While there are many, many libraries available, the goal of this implementation is to make *usage* easy.

## Features
- Literals support so you can use `auto u= "https://www.siddiqsoft.com/"_Uri;`
- Formatters for `std::format` as well as serializers for `nlohmann.json` library.
- Support for `std::string` and `std::wstring`
- Cross-platform header-only with googletest framework for tests. _Build and tests are for Visual Studio 2019 under x64._

Please refer to the [documentation](https://siddiqsoft.github.io/SplitUri/).

## Usage

> Breaking change with `v1.9.0` the literals namespace has been changed to `siddiqsoft::splituri_literals`.

<p align="right">
&copy; 2021 Siddiq Software LLC. All rights reserved.
</p>
