module MRuby
  module Sandbox
    class Hash
      include Enumerable
      attr_reader :hash

      def initialize
        @hash = {}
      end

      def [](name)
        @hash[name]
      end

      def []=(name, value)
        @hash[name] = value
      end

      def each(&block)
        @hash.each(&block)
      end

      def each_key(&block)
        @hash.each_key(&block)
      end

      def check
        sandbox_names = @hash.keys
        sandbox_names.each do |name|
          @hash[name].uniq!
        end
        @hash.each do |name, sandbox|
          gem_names_in_sandbox = sandbox.gems.map { |g| g.name }
          sandbox.each do |gem|
            gem.setup
            if gem.dependencies
              gem.dependencies.each do |dep|
                gem_name = dep[:gem]
                unless gem_names_in_sandbox.include? gem_name
                  fail %Q[GEM #{gem_name} not found in the sandbox `#{name}']
                end
              end
            end
          end
        end
      end
    end # Hash

    class Sandbox
      include Enumerable

      attr_accessor :name
      attr_reader :gems

      def initialize(name)
        @name = name
        @gems = []
      end

      def config=(obj)
        @config = obj
      end

      def each(&block)
        @gems.each(&block)
      end

      def include?(gem)
        @gems.include? gem
      end

      def uniq!
        @gems.uniq!
      end

      def gem(gemdir, &block)
        g = @config.gem(gemdir, &block)
        @gems << g
      end
    end # Sandbox
  end # Sandbox
end
