assert('mrb_str_to_cstr()') do
  result = true

  begin
    CAPITest_mrb_str_to_cstr::mrb_str_to_cstr(1)
    result = false
  rescue TypeError
  end

  begin
    CAPITest_mrb_str_to_cstr::mrb_str_to_cstr("asdfasfd\0asdfasfd")
    result = false
  rescue ArgumentError
  end

  result
end
