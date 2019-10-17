@echo off
clang-cl -Z7 -orelease/voidc.exe -Isource source/app/*.cpp -Wno-c++17-extensions