module MRuby
  module ThreadBind
    class List
      include Enumerable
      attr_accessor :ary

      def initialize
        @ary = []
      end

      def each(&block)
        @ary.each(&block)
      end

      def <<(thread)
        @ary << thread
      end

      def length
        @ary.length
      end
    end


    class ThreadBind
      attr_accessor :activate
      attr_accessor :script_path
      attr_accessor :sandbox
      attr_accessor :priority
      attr_accessor :stack_size
      attr_accessor :type

      def initialize
        @activate = false
        @script_path = nil
        @sandbox = nil
        @priority = 0
        @stack_size = -1
        @type = nil
      end

      def config=(obj)
        @config = obj
      end
    end
  end
end
