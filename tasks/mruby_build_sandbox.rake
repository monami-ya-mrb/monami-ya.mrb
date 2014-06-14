module MRuby
  module Sandbox
    def sandbox(name, &block)
      fail "Sandbox named #{name} is already available." if sandboxes[name]
      sandbox = Sandbox.new(name)
      sandbox.config = self
      @sandboxes[name] = sandbox
      sandbox.instance_eval(&block)
    end
  end
end

