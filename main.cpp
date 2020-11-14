

#include <iostream>
#include "wiz/ClauText.h"
#include <stack>
#include <queue>

using namespace std::literals;


namespace Lint {
	bool print_off = false; // print yes or no?
	wiz::Option opt;

	class Option
	{
	public:
		enum class Type_ { ANY, INT, FLOAT, STRING };
		enum class Required_ { REQUIRED, OPTIONAL_ };

		//	enum class Multiple_ { NONE, OFF, ON };
	public:
		std::vector<Type_> type;
		std::vector<std::string> ids;
		Required_ required;
		//Multiple_ multiple;

		std::vector<std::string> event_ids;
		std::vector<std::string> enum_ids;
		std::vector<std::string> style_ids;

		std::string prefix;

		wiz::DataType minimum, maximum;
		bool use_minimum = false, use_maximum = false;
		long long minItems = 0, maxItems = 0;
		bool use_minItems = false, use_maxItems = false;

		std::string regex;

		std::vector<std::string> needs;
	public:
		Option() : type(),
			required(Required_::REQUIRED)
			//,	multiple(Multiple_::OFF)
		{
			//
		}
	public:
		//	Option& Multiple(Multiple_ multiple) {
			//	this->multiple = multiple;
		//		return *this;
		//	}
		Option& Type(Type_ type) {
			this->type.push_back(type);
			return *this;
		}
		Option& Id(const std::string& id) {
			this->ids.push_back(id);
			return *this;
		}
		Option& Required(Required_ required) {
			this->required = required;
			return *this;
		}
		Option& Event(const std::string& event_id) {
			this->event_ids.push_back(event_id);
			return *this;
		}
		Option& Enum(const std::string& enum_id) {
			this->enum_ids.push_back(enum_id);
			return *this;
		}
		Option& Style(const std::string& style_id) {
			this->style_ids.push_back(style_id);
			return *this;
		}
	public:
		// check valid function?
		bool IsValid() const {
			// to do
			return false;
		}
	public:
		Option operator+(const Option& other) const {
			Option temp = *this;

			for (size_t i = 0; i < other.event_ids.size(); ++i) {
				temp.event_ids.push_back(other.event_ids[i]);
			}
			for (size_t i = 0; i < other.enum_ids.size(); ++i) {
				temp.enum_ids.push_back(other.enum_ids[i]);
			}
			for (size_t i = 0; i < other.style_ids.size(); ++i) {
				temp.style_ids.push_back(other.style_ids[i]);
			}
			for (size_t i = 0; i < other.ids.size(); ++i) {
				temp.ids.push_back(other.ids[i]);
			}

			//if (other.multiple == Multiple_::ON) {
			//	temp.multiple = Multiple_::ON;
			//}


			if (other.required == Required_::OPTIONAL_) {
				temp.required = Required_::OPTIONAL_;
			}
			for (const auto& x : other.type) {
				temp.type.push_back(x);
			}

			if (other.use_minimum) {
				temp.use_minimum = true;
				temp.minimum = other.minimum;
			}
			if (other.use_maximum) {
				temp.use_maximum = true;
				temp.maximum = other.maximum;
			}

			if (other.use_minItems) {
				temp.use_minItems = true;
				temp.minItems = other.minItems;
			}
			if (other.use_maxItems) {
				temp.use_maxItems = true;
				temp.maxItems = other.maxItems;
			}
			if (!other.regex.empty()) {
				temp.regex = other.regex;
			}

			for (size_t i = 0; i < other.needs.size(); ++i) {
				temp.needs.push_back(other.needs[i]);
			}

			return temp;
		}
	};


	// itemtype, usertype flag?
	Option OptionFrom(const std::string& option_str, wiz::load_data::UserType* mainUT, const std::map<std::string, wiz::load_data::UserType*>& styleMap)
	{
		Option option;

		std::string::size_type start = 0;
		std::string::size_type find_percent = std::string::npos; // % : delimeter.

		if ((find_percent = option_str.find('%', start)) == std::string::npos)
		{
			option.prefix = option_str;
		}
		else {
			option.prefix = option_str.substr(start, find_percent - start);
		}

		while ((find_percent = option_str.find('%', start)) != std::string::npos)
		{
			std::string::size_type end_ = option_str.find('%', find_percent + 1);

			start = find_percent; // ex) abc%id?

			if (end_ == std::string::npos) {
				end_ = option_str.size();
			}

			const std::string opt = option_str.substr(start, end_ - 1 - start + 1);

			if (wiz::String::startsWith(opt, "%int"sv)) {
				option.Type(Option::Type_::INT);

				std::vector<std::string> temp = wiz::tokenize(opt, '@');

				if (temp.size() > 1) {
					std::string argument = temp[1].substr(1, temp[1].size() - 2);
					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(argument, ut);

					auto item1 = ut.GetItem("minimum");
					if (item1.size() > 0) {
						option.use_minimum = true;
						option.minimum = item1[0].Get();
					}
					auto item2 = ut.GetItem("maximum");
					if (item2.size() > 0) {
						option.use_maximum = true;
						option.maximum = item2[0].Get();
					}
				}
			}
			else if (wiz::String::startsWith(opt, "%float"sv)) {
				option.Type(Option::Type_::FLOAT);

				std::vector<std::string> temp = wiz::tokenize(opt, '@');

				if (temp.size() > 1) {
					std::string argument = temp[1].substr(1, temp[1].size() - 2);
					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(argument, ut);

					auto item1 = ut.GetItem("minimum");
					if (item1.size() > 0) {
						option.use_minimum = true;
						option.minimum = item1[0].Get();
					}
					auto item2 = ut.GetItem("maximum");
					if (item2.size() > 0) {
						option.use_maximum = true;
						option.maximum = item2[0].Get();
					}
				}
			}
			else if ("%string"sv == opt) {
				option.Type(Option::Type_::STRING);
			}
			else if (wiz::String::startsWith(opt, "%id_")) {
				option.Id(wiz::String::substring(opt, 4));
			}
			else if ("%any"sv == opt) {
				option.Type(Option::Type_::ANY);
			}
			else if ("%optional"sv == opt) {
				option.Required(Option::Required_::OPTIONAL_);
			}
			else if ("%required"sv == opt) {
				option.Required(Option::Required_::REQUIRED);
			}
			//	else if ("%multiple"sv == opt) {
			//		option.Multiple(Option::Multiple_::ON);
			//	}
			else if (wiz::String::startsWith(opt, "%minItems"sv)) { // size check?
				std::string argument;

				{
					std::vector<std::string> temp = wiz::tokenize(opt, '@');
					if (temp.size() > 1) {
						argument = temp[1].substr(1, temp[1].size() - 2);

						wiz::DataType x(argument);
						option.use_minItems = true;
						option.minItems = x.ToInt();
					}
				}

			}
			else if (wiz::String::startsWith(opt, "%maxItems"sv)) { // size check?
				std::string argument;

				{
					std::vector<std::string> temp = wiz::tokenize(opt, '@');
					if (temp.size() > 1) {
						argument = temp[1].substr(1, temp[1].size() - 2);

						wiz::DataType x(argument);
						option.use_maxItems = true;
						option.maxItems = x.ToInt();
					}
				}
			}
			else if (wiz::String::startsWith(opt, "%regex"sv)) {
				std::string expr;

				std::vector<std::string> temp = wiz::tokenize(opt, '@');
				if (temp.size() > 1) {
					expr = temp[1].substr(1, temp[1].size() - 2);

					option.regex = expr;
				}
			}
			else if (wiz::String::startsWith(opt, "%need"sv)) {
				std::string name;

				std::vector<std::string> temp = wiz::tokenize(opt, '@');

				if (temp.size() > 1) {
					name = temp[1].substr(1, temp[1].size() - 2);

					option.needs.push_back(name);
				}
			}
			else if (wiz::String::startsWith(opt, "%event_"sv)) { // size check?
				std::string event_name = wiz::String::substring(opt, 7);
				option.Event(std::move(event_name));
			}
			else if (wiz::String::startsWith(opt, "%enum_"sv)) { // size check?
				std::string enum_name = wiz::String::substring(opt, 6);
				option.Enum(std::move(enum_name));
			}
			else if (wiz::String::startsWith(opt, "%style_"sv)) {
				std::string style_id;
				std::string argument;
				wiz::ExecuteData executeData;

				{
					std::vector<std::string> temp = wiz::tokenize(opt.substr(7), '@');
					style_id = temp[0];
					if (temp.size() > 1) {
						argument = temp[1].substr(1, temp[1].size() - 2);
					}

					wiz::DataType x = wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, argument, wiz::ExecuteData()).ToString();
					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(x.ToString(), ut);

					for (size_t i = 0; i < ut.GetItemListSize(); ++i) {
						executeData.info.parameters.insert({ ut.GetItemList(i).GetName().ToString(), ut.GetItemList(i).Get().ToString() });
					}
				}

				std::string result;

				auto styleUT = styleMap.at(style_id);

				for (size_t i = 0; i < styleUT->GetItemListSize(); ++i) {
					result += styleUT->GetItemList(i).Get().ToString();
				}

				for (size_t i = 0; i < styleUT->GetUserTypeListSize(); ++i) {
					if (styleUT->GetUserTypeList(i)->GetName() == "$parameter"sv) {
						continue;
					}
					result += styleUT->GetUserTypeList(i)->GetName().ToString();
					wiz::load_data::UserType temp = *(styleUT->GetUserTypeList(i));

					result += "@\'" + wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, temp, executeData).ToString();
					result += "\'";
				}

				option = option + OptionFrom(result, mainUT, styleMap);
			}
			else {
				std::cout << "wrong option" << ENTER;
			}

			start = end_;
		}

		if (option.type.empty()) {
			option.type.push_back(Option::Type_::ANY);
		}
		return option;
	}

	inline bool OptionDoA(const Option& option, std::string_view str) 
	{
		if (option.prefix.empty() == false &&
			option.prefix == str) {
			return true;
		}

		long long count = option.type.size();
		auto type_list = option.type;

		while (!type_list.empty()) {
			switch (type_list.back()) {
			case Option::Type_::ANY:
				// pass
				break;
			case Option::Type_::INT:
				if (wiz::load_data::Utility::IsIntegerInJson(str)) {
					if (option.use_minimum) {
						wiz::DataType x(str.data());
						if (option.minimum.ToInt() <= x.ToInt()) {
							//
						}
						else {
							std::cout << str << " is small" << ENTER;
							count--;
						}
					}
					if (option.use_maximum) {
						wiz::DataType x(str.data());
						if (option.maximum.ToInt() >= x.ToInt()) {
							//
						}
						else {
							std::cout << str << " is big" << ENTER;
							count--;
						}
					}
				}
				else {
					std::cout << str << " is not integer" << ENTER;
					count--;
				}
				break;
			case Option::Type_::FLOAT:
				if (wiz::load_data::Utility::IsFloatInJson(str)) {
					if (option.use_minimum) {
						wiz::DataType x(str.data());
						if (option.minimum.ToFloat() <= x.ToFloat()) {
							//
						}
						else {
							std::cout << str << " is small" << ENTER;
							count--;
						}
					}
					if (option.use_maximum) {
						wiz::DataType x(str.data());
						if (option.maximum.ToFloat() >= x.ToFloat()) {
							//
						}
						else {
							std::cout << str << " is big" << ENTER;
							count--;
						}
					}
				}
				else {
					std::cout << str << " is not float" << ENTER;
					count--;
				}
				break;

			case Option::Type_::STRING:
				/* // quoted string.
				if (str.size() >= 2 && str[0] == str.back() && str.back() == '\"') { // cf '\''
					//
				}
				else {
					std::cout << str << " is not quoted string" << ENTER;
					count--;
				}
				*/
				break;
			}
			type_list.pop_back();
		}

		return count > 0;
	}


	class ValidationInfo {
	public:
		const wiz::load_data::UserType* nowTextUT;
		const wiz::load_data::UserType* nowSchemaUT;

		size_t index;
		size_t itCount;
		size_t utCount;

		std::vector<int> visited; // for nowTextUT.
		bool isOrdered = false; // object -> false?, array -> true?, and mixed?

	public:
		ValidationInfo(const wiz::load_data::UserType* nowTextUT, const wiz::load_data::UserType* nowSchemaUT)
			: nowTextUT(nowTextUT), nowSchemaUT(nowSchemaUT), index(0), itCount(0), utCount(0)
		{

		}
	};


	bool Validate(const wiz::load_data::UserType* textUT, const wiz::load_data::UserType* schemaUT, wiz::load_data::UserType* eventUT, 
			const std::map<std::string, wiz::load_data::UserType*>& styleMap) {
		std::queue<ValidationInfo> que;

		que.push(ValidationInfo(textUT, schemaUT));

		while (!que.empty()) {
			auto iter = que.front();
			que.pop();

			// schemaUT..
			size_t i = iter.index;
			if (i < iter.nowSchemaUT->GetIListSize()) {
				if (iter.nowSchemaUT->IsItemList(i)) {
					// key = val or val
					Option opt_key = OptionFrom(iter.nowSchemaUT->GetItemList(iter.itCount).GetName().ToString(), eventUT, styleMap);
					Option opt_data = OptionFrom(iter.nowSchemaUT->GetItemList(iter.itCount).Get().ToString(), eventUT, styleMap);


					// DoA - chk minimum, maximum, type, regex?, enum? 
					// DoB - chk event?
					// chk optional, required, need?
					// print_off?


					iter.index++;
					iter.itCount++;

					que.push(iter);
				}
				else { // usertype..
					// key = { } or  { }
					Option opt_key = OptionFrom(iter.nowSchemaUT->GetItemList(iter.itCount).GetName().ToString(), eventUT, styleMap);

					// DoA - chk minimum, maximum, type, regex?, enum? 
					// DoB - chk event?
					// chk optional, required, need?
					// chk minItems, maxItems, 
					// print_off?

					iter.index++;
					iter.utCount++;

					// push to stack.
					// chk now text ut -> child

					que.push(iter);
					que.push(Lint::ValidationInfo(iter.nowTextUT->GetUserTypeList(iter.utCount - 1), iter.nowSchemaUT->GetUserTypeList(iter.utCount - 1)));
				}
			}
			else { // i == iter.nowSchemaUT->GetIListSize()
				// chk visited...
			}
		}


		return true;
	}
}


int main(int argc, char* argv[]) // program_name text_file schema_file
{
	if (argc != 3) {
		std::cout << "text_file schema_file\n";
		return -1;
	}

	try {

		std::string text_file = argv[1];
		std::string schema_file = argv[2];

		wiz::load_data::UserType textUT, schemaUT;

		if (!wiz::load_data::LoadData::LoadDataFromFile(text_file, textUT)) {
			std::cout << "loading text file is failed\n";
			return -2;
		}
		if (!wiz::load_data::LoadData::LoadDataFromFile(schema_file, schemaUT)) {
			std::cout << "loading schema file is failed\n";
			return -3;
		}

		// call __init__ Event...


		// chk schemaUT has  Text = { }
		auto x = schemaUT.GetUserTypeItem("Text");

		if (x.empty()) {
			std::cout << "schema is not valid\n";
			return -4;
		}

		auto styleList = schemaUT.GetUserTypeItem("Style");
		std::map<std::string, wiz::load_data::UserType*> style_map;

		for (auto x : styleList) {
			style_map.insert(std::make_pair(x->GetItem("id")[0].Get().ToString(), x));
		}


		if (!Lint::Validate(&textUT, x[0], &schemaUT, style_map)) {
			std::cout << "text file is not valid with schema\n";
			return -5;
		}


		// call __end__ Event...


	}
	catch (...) {
		std::cout << "internal error\n";
		return -100;
	}

	return 0;
}

