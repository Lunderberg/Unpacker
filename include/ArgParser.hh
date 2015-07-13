#ifndef _ARGPARSER_H_
#define _ARGPARSER_H_

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct ParseError : public std::runtime_error{
  ParseError(const char* msg) : std::runtime_error(msg) { }
  ParseError(const std::string& msg) : std::runtime_error(msg) { }
};

// struct ParseError : public std::runtime_error{
//   using std::runtime_error::runtime_error;
// };

class ArgParseItem{
public:
  ArgParseItem() : present_(false) { }
  virtual ~ArgParseItem(){ }
  virtual bool matches(const std::string& flag, bool multichar_flag) const = 0;
  virtual void parse_item(const std::vector<std::string>& arguments) = 0;
  virtual int num_arguments() const = 0;
  virtual std::string printable(int description_column = -1, int* chars_before_desc=NULL) const = 0;
  virtual bool is_required() const = 0;
  bool is_present() const {return present_;}
  virtual std::string flag_name() const = 0;

  void parse(const std::string& name, const std::vector<std::string>& arguments){
    if((num_arguments()==-1 && arguments.size()==0) ||
       (num_arguments()!=-1 && arguments.size() != size_t(num_arguments()))){
      std::stringstream ss;
      if(num_arguments()==-1){
        ss << "Flag \"" << name << "\" expected at least one argument";
      } else {
        ss << "Flag \"" << name << "\" expected " << num_arguments()
           << " argument(s) and received " << arguments.size();
      }
      throw ParseError(ss.str());
    }

    present_ = true;
    parse_item(arguments);
  }

private:
  bool present_;
};

template<typename T>
class ArgParseConfig : public ArgParseItem {
public:
  ArgParseConfig(std::string flag_list)
    : desc(""), required_(false) {
    std::stringstream ss(flag_list);
    while(!ss.eof()){
      std::string temp;
      ss >> temp;
      flags.push_back(temp);
    }
  }
  virtual ~ArgParseConfig(){ }

  virtual std::string flag_name() const {
    std::string output;
    for(auto flag : flags){
      if(flag.length() > output.length()){
        output = flag;
      }
    }
    return output;
  }

  virtual bool matches(const std::string& flag, bool multichar_flag) const {
    for(auto& f : flags){
      bool is_multichar = f.length()>1;
      if(f == flag &&
         multichar_flag == is_multichar ){
        return true;
      }
    }
    return false;
  }

  virtual ArgParseConfig& description(const std::string& d){
    desc = d;
    return *this;
  }

  virtual ArgParseConfig& required(){
    required_ = true;
    return *this;
  }

  virtual bool is_required() const {
    return required_;
  }

  virtual ArgParseConfig& default_value(T value) = 0;

  virtual std::string printable(int description_column = -1, int* chars_before_desc=NULL) const {
    std::stringstream ss;

    ss << "  ";

    bool has_singlechar_flag = false;
    for(auto flag : flags){
      if(flag.length()==1){
        ss << "-" << flag << " ";
        has_singlechar_flag = true;
      }
    }
    for(auto flag : flags){
      if(flag.length()!=1){
        if(has_singlechar_flag){
          ss << "[ ";
        }
        ss << "--" << flag;
        if(has_singlechar_flag){
          ss << " ]";
        }
      }
    }

    if(num_arguments()!=0){
      ss << " arg ";
    }

    auto chars = ss.tellp();
    if(chars_before_desc){
      *chars_before_desc = chars;
    }

    if(description_column != -1 &&
       chars < description_column){
      for(unsigned int i=0; i<description_column-chars; i++){
        ss << " ";
      }
    }

    ss << desc;

    return ss.str();
  }

protected:
  std::string desc;
  std::vector<std::string> flags;
  bool required_;
};

template<typename T>
class ArgParseConfigT : public ArgParseConfig<T> {
public:
  ArgParseConfigT(std::string flag, T* output_location)
    : ArgParseConfig<T>(flag), output_location(output_location) { }

  virtual ArgParseConfig<T>& default_value(T value){
    *output_location = value;
    return *this;
  }

  virtual void parse_item(const std::vector<std::string>& arguments){
    std::stringstream ss(arguments[0]);
    ss >> *output_location;
  }

  virtual int num_arguments() const { return 1; }


private:
  T* output_location;
};

template<>
class ArgParseConfigT<bool> : public ArgParseConfig<bool> {
public:
  ArgParseConfigT(std::string flag, bool* output_location)
    : ArgParseConfig<bool>(flag), output_location(output_location) {
    *output_location = false;
  }

  virtual ArgParseConfig<bool>& default_value(bool value){
    *output_location = value;
    stored_default_value = value;
    return *this;
  }

  virtual void parse_item(const std::vector<std::string>& /*arguments*/){
    *output_location = !stored_default_value;
  }

  virtual int num_arguments() const { return 0; }

private:
  bool* output_location;
  bool stored_default_value;
};

template<typename T>
class ArgParseConfigT<std::vector<T> > : public ArgParseConfig<std::vector<T> > {
public:
  ArgParseConfigT(std::string flag, std::vector<T>* output_location)
    : ArgParseConfig<std::vector<T> >(flag), output_location(output_location),
      num_arguments_expected(-1) { }

  virtual void parse_item(const std::vector<std::string>& arguments){
    for(auto arg : arguments){
      std::stringstream ss(arg);
      T val;
      ss >> val;
      output_location->push_back(val);
    }
  }

  virtual int num_arguments() const { return num_arguments_expected; }

  virtual ArgParseConfig<std::vector<T> >& default_value(std::vector<T> value){
    *output_location = value;
    return *this;
  }

private:
  std::vector<T>* output_location;
  int num_arguments_expected;
};

class ArgParser {
public:
  ArgParser() { }

  ~ArgParser() {
    for(auto val : values){
      delete val;
    }
  }

  void parse(int argc, char** argv) {
    int iarg = 1;
    while(iarg < argc){

      std::string arg = argv[iarg++];
      if(arg.at(0) != '-'){
        std::stringstream ss;
        ss << "Expected flag beginning with '-', received \"" << arg << "\" instead.";
        throw ParseError(ss.str());
      }

      if(arg.at(1) == '-'){
        handle_long_flag(argc, argv, iarg);
      } else {
        handle_short_flag(argc, argv, iarg);
      }
    }

    for(auto val : values){
      if(val->is_required() && !val->is_present()){
        std::stringstream ss;
        ss << "Required argument \"" << val->flag_name() << "\" is not present";
        throw ParseError(ss.str());
      }
    }
  }

  template<typename T>
  ArgParseConfigT<T>& option(const std::string flag, T* output_location){
    ArgParseConfigT<T>* output = new ArgParseConfigT<T>(flag, output_location);
    values.push_back(output);
    return *output;
  }

  void print(std::ostream& out) const {
    out << "Options:\n";

    int max_length = -1;
    for(auto item : values){
      int length;
      item->printable(-1, &length);
      max_length = std::max(length, max_length);
    }

    for(auto it = values.begin(); it!=values.end(); it++){
      ArgParseItem* item = *it;
      out << item->printable(max_length);
      if(it!=values.end()-1){
        out << "\n";
      }
    }
  }

private:
  void handle_long_flag(int argc, char** argv, int& iarg){
    std::string arg = argv[iarg-1];
    std::vector<std::string> flag_args;
    std::string flag;
    size_t equals_index = arg.find("=");
    if(equals_index == std::string::npos){
      // flag followed by list of flag_args
      flag = arg.substr(2);
      flag_args = argument_list(argc, argv, iarg);
    } else {
      // = inside flag
      flag = arg.substr(2, equals_index-2);
      flag_args.push_back(arg.substr(equals_index+1));
    }
    ArgParseItem& item = get_item(flag, true);
    item.parse(flag, flag_args);
  }

  void handle_short_flag(int argc, char** argv, int& iarg){
    std::string arg = argv[iarg-1];
    std::string flag = arg.substr(1,1);
    ArgParseItem& item = get_item(flag, false);
    if(item.num_arguments() == 0){
      // Each character is a boolean flag
      for(unsigned int ichar=1; ichar<arg.length(); ichar++){
        std::string flag = arg.substr(ichar,1);
        std::vector<std::string> flag_args;
        get_item(flag, false).parse(flag, flag_args);
      }
    } else {
      if(arg.length() == 2){
        // Next arguments passed to the program get passed to the flag.
        std::vector<std::string> flag_args = argument_list(argc, argv, iarg);
        item.parse(flag, flag_args);
      } else {
        // Everything past the first character is argument to the flag.
        std::vector<std::string> flag_args{arg.substr(2)};
        item.parse(flag, flag_args);
      }
    }
  }

  //! Reads arguments into a list until finding one that begins with '-'
  std::vector<std::string> argument_list(int argc, char** argv, int& iarg){
    std::vector<std::string> output;
    bool read_extra = false;
    while(iarg<argc){
      std::string next_arg = argv[iarg++];
      if(next_arg.at(0) == '-'){
        read_extra = true;
        break;
      } else {
        output.push_back(next_arg);
      }
    }
    if(read_extra){
      iarg--;
    }
    return output;
  }

  ArgParseItem& get_item(const std::string& flag, bool multichar_flag){
    for(auto val : values){
      if(val->matches(flag, multichar_flag)){
        return *val;
      }
    }

    std::stringstream ss;
    ss << "Unknown option: \"" << flag << "\"";
    throw ParseError(ss.str());
  }

  std::vector<ArgParseItem*> values;
};

std::ostream& operator<<(std::ostream& out, const ArgParser& val){
  val.print(out);
  return out;
}

#endif /* _ARGPARSER_H_ */
