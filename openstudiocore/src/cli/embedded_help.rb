
module RbConfig
  def RbConfig.ruby
    EmbeddedScripting::applicationFilePath;
  end
end

module OpenStudio
  def self.openstudio_path
    RbConfig.ruby
  end
end

BINDING = Kernel::binding()
Encoding.default_external = Encoding::ASCII

module Kernel
  # ":" is our root path to the embedded file system
  # make sure it is in the ruby load path
  $LOAD_PATH << ':'
  $LOAD_PATH << ':/ruby/2.0.0'
  $LOAD_PATH << ':/ruby/2.0.0/x86_64-darwin13.4.0'
  $LOAD_PATH << ':/ruby/2.0.0/x64-mswin64_120'
  $LOAD_PATH << EmbeddedScripting::findFirstFileByName('openstudio-standards.rb').gsub('openstudio-standards.rb', '')
  $LOAD_PATH << EmbeddedScripting::findFirstFileByName('openstudio-workflow.rb').gsub('openstudio-workflow.rb', '')
  $LOADED = []

  alias :original_require_relative :require_relative
  alias :original_require :require

  def require path
    rb_path = path

    if path.include? 'openstudio/energyplus/find_energyplus'
      return false
    end

    jsonparser = 'json/ext/parser' 
    if path == jsonparser
      if $LOADED.include?(jsonparser) then
        return true
      else
        EmbeddedScripting::initJSONParser()
        $LOADED << jsonparser
        return true
      end
    end

    jsongenerator = 'json/ext/generator'
    if path == jsongenerator
      if $LOADED.include?(jsongenerator) then
        return true
      else
        EmbeddedScripting::initJSONGenerator()
        $LOADED << jsongenerator
        return true
      end
    end

    extname = File.extname(path)
    if extname.empty? or extname != '.rb'
      rb_path = path + '.rb'
    end 

    if rb_path.chars.first == ':'
       if $LOADED.include?(rb_path) then
         return true
      else
        return require_embedded_absolute(rb_path)
      end
    #elsif rb_path == 'openstudio.rb'
    #  return true
    else
      $LOAD_PATH.each do |p|
        if p.chars.first == ':' then
          embedded_path = p + '/' + rb_path
          if $LOADED.include?(embedded_path) then
            return true
          elsif EmbeddedScripting::hasFile(embedded_path) then
            return require_embedded_absolute(embedded_path)
          end
        end
      end
    end

    return original_require path
  end

  def require_embedded_absolute path
    $LOADED << path
    result = eval(EmbeddedScripting::getFileAsString(path),BINDING,path)
    return result
  end

  def require_relative path
    absolute_path = File.dirname(caller_locations(1,1)[0].path) + '/' + path
    if absolute_path.chars.first == ':'
      absolute_path[0] = ''
      absolute_path = File.expand_path absolute_path
      
      # strip Windows drive letters
      if /[A-Z\:]/.match(absolute_path)
        absolute_path = absolute_path[2..-1]
      end
      absolute_path = ':' + absolute_path
    end
    return require absolute_path
  end
  
  # this function either reads a file from the embedded archive or from disk, returns file content as a string
  def load_resource_relative(path, mode='r')

    absolute_path = File.dirname(caller_locations(1,1)[0].path) + '/' + path
    if absolute_path.chars.first == ':'
      absolute_path[0] = ''
      absolute_path = File.expand_path absolute_path
      
      # strip Windows drive letters
      if /[A-Z\:]/.match(absolute_path)
        absolute_path = absolute_path[2..-1]
      end
      absolute_path = ':' + absolute_path
    end
    
    if EmbeddedScripting::hasFile(absolute_path)
      return EmbeddedScripting::getFileAsString(absolute_path)
    end
    
    result = ""
    if File.exists?(path)
      File.open(path, mode) do |file|
        result = file.read
      end
    end
    return result
  end
  
end

