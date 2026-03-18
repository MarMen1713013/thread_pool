type_check_test = executable(
  'type_check_test',
  sources: files('type_check_test.cpp'),
  dependencies: thread_safe_dep,
  install: false,
)
test(
  'type_check',
  type_check_test,
)

thread_safe_queue = executable(
  'thread_safe_queue',
  sources: files('thread_safe_queue.cpp'),
  dependencies: thread_safe_dep,
  install: false,
)

test(
  'thread_safe_queue',
  thread_safe_queue,
)
