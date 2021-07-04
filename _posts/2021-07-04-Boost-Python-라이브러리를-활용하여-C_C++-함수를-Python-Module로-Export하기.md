---
layout: post
title:  "Boost Python 라이브러리를 활용하여 C/C++ 함수를 Python Module로 Export하기"
date:   2021-07-04 21:08:00 +0900
categories: C/C++ Python Boost
---

# Overview
Microsoft Visual Studio 에서 Boost Python 라이브러리를 활용하여 C/C++ 함수(라이브러리)를 Python 모듈로 Export 하는 방법을 알아본다.

# Boost C++ Library 설치
## 1. Boost C++ Library 1.76.0 버전을 다운로드한다.
Boost 1.76.0 버전은 https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/ 에서 다운 받을 수 있다.<br>
위 링크에서 [boost_1_76_0.7z][boost_1_76_0.7z_link] 혹은 [boost_1_76_0.zip][boost_1_76_0.zip_link] 을 다운로드 받은 뒤 압축을 풀어 준다.

## 2. Boost Python Library를 빌드한다.
Boost 라이브러리는 기본적으로 Header-only library로 빌드가 따로 필요없지만 Boost Python 라이브러리의 경우는 빌드가 필요하다.<br>
<br>
압축을 푼 boost_1_76_0 폴더로 이동한 뒤 `bootstrap.bat` 을 실행한다.(Windows 기준)<br>
실행이 완료되면 `b2.exe` 파일이 생성된 것을 확인할 수 있다.<br>
<br>
빌드에 사용할 Python 환경을 설정하기 위해 `project-config.jam` 파일을 연다.<br>
`project-config.jam` 파일에 다음과 같은 구문을 추가한다.
``` jam
using python : [version] : [command-or-prefix] : [includes] : [libraries] : [conditions] : [extension-suffix] ;
```
위 구문에 대한 설명은 [여기](https://www.boost.org/doc/libs/1_76_0/tools/build/doc/html/index.html#bbv2.reference.tools.libraries.python)에서 확인할 수 있다.<br>
<br>
필자는 anaconda의 64 bit 3.8 버전의 파이썬을 사용하였으며 다음의 내용을 `project-config.jam`에 추가하였다.
``` jam
using python
: 3.8 # version
: C:/Users/tpdus/anaconda3/python.exe # Interperter/path to dir
: C:/Users/tpdus/anaconda3/include # includes
: C:/Users/tpdus/anaconda3/libs # libs
: <address-model>64 # conditions
;
```
<br>

커맨드 창에서 `boost_1_76_0` 폴더로 이동한 뒤 다음 명령어를 실행한다.
```
.\b2.exe toolset=msvc --build-type=complete --with-python stage
```
<br>

빌드가 성공적으로 완료되면 `boost_1_76_0/stage/lib` 에 `boost_python`으로 시작되는 `.dll`, `.lib` 파일들이 생성된 것을 확인할 수 있다.<br>
<br>
B2의 자세한 사용방법은 [여기](https://www.boost.org/doc/libs/1_76_0/tools/build/doc/html/index.html)에서 확인할 수 있다.


[boost_1_76_0.7z_link]: https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.7z
[boost_1_76_0.zip_link]: https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.zip