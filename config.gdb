set breakpoint pending on
set confirm off
file ./cronac
break crona::Err::report
commands
	where
end
break crona::InternalError::InternalError
commands
	where
end

define p3
  set args p3_tests/$arg0.crona -p --
  run
end
