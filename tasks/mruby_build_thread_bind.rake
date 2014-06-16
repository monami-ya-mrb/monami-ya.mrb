module MRuby
  module ThreadBind
    def thread(&block)
      thread_bind = ThreadBind.new
      thread_bind.config = self
      thread_bind.instance_eval(&block)
      thread_binds << thread_bind
    end
  end
end

