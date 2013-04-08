assert('mrb_panic_{set,reset}(): state local / normal operation.') do
  CAPITest_mrb_panic::test1
end

assert('mrb_panic_{set,reset}(): state local / resets with invalid flow.') do
  begin
    CAPITest_mrb_panic::test2
  rescue ArgumentError
    true
  end
end

assert('mrb_panic_{set,reset}(): global / normal operation.') do
  CAPITest_mrb_panic::test3
end

assert('mrb_panic_{set,reset}(): global / resets with invalid flow.') do
  CAPITest_mrb_panic::test4
end

assert('mrb_panic(): state local') do
 begin
   CAPITest_mrb_panic::test5
 rescue RuntimeError
   true
 end
end

assert('mrb_panic(): global') do
 CAPITest_mrb_panic::test6
end

# We don't provide test case for default abort.
# The reason is obvious, isn't it?
