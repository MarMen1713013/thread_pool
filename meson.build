project(
  'thread_pool',
  'cpp',
  version: '0.1.0',
  default_options: [
    'cpp_std=c++17',
    'buildtype=release',
    ]
  ) 

thread_dep = dependency('threads')

thread_safe_lib = library(
  'thread_safe',
  sources: files('src/thread_safe.cpp'),
  include_directories: include_directories('include'),
  dependencies: [thread_dep],
  install: false,
  )

thread_safe_dep = declare_dependency(
  link_with: thread_safe_lib,
  include_directories: include_directories('include'),
  dependencies: [thread_dep],
  )

subdir('src/example')
subdir('src/tests')
