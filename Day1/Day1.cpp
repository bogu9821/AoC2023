#include <print>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <numeric>
#include <chrono>

class Lexer
{
public:
	virtual ~Lexer() = default;
	virtual std::array<char, 2> ParseDigits(std::string_view const t_text) const = 0;
};

class DigitsLexer : public Lexer
{
public:
	virtual ~DigitsLexer() = default;

	bool HandleDigits(char const t_character, std::array<char, 2>& t_digits) const
	{
		if (!IsDigit(t_character))
		{
			return false;
		}

		if (t_digits[0] == 0)
		{
			t_digits[1] = t_digits[0] = t_character;
		}
		else
		{
			t_digits[1] = t_character;
		}

		return true;
	}

	std::array<char, 2> ParseDigits(std::string_view const t_text) const override
	{
		std::array<char, 2> digits{};

		for (size_t i = 0; i < std::size(t_text); i++)
		{
			(void)HandleDigits(t_text[i], digits);
		}
		return digits;
	}

	static bool IsDigit(char const t_character)
	{
		return (t_character >= '0' && t_character <= '9');
	}
};

class TextLexer final : public DigitsLexer
{
public:
	~TextLexer() override = default;

	std::array<char, 2> ParseDigits(std::string_view const t_text) const override
	{
		using namespace std::literals::string_view_literals;
		static constexpr std::array digitsStr =
		{
			"one"sv,
			"two"sv,
			"three"sv,
			"four"sv,
			"five"sv,
			"six"sv,
			"seven"sv,
			"eight"sv,
			"nine"sv
		};

		std::array<char, 2> digits{};

		for (size_t i = 0; i < std::size(t_text); i++)
		{
			if (HandleDigits(t_text[i], digits))
			{
				continue;
			}

			for (size_t j = 0; j < std::size(digitsStr); j++)
			{
				if (t_text.substr(i, digitsStr[j].size()) == digitsStr[j])
				{
					const char value = static_cast<char>(j) + '1';

					if (digits[0] == 0)
					{
						digits[1] = digits[0] = value;
					}
					else
					{
						digits[1] = value;
					}
				}
			}
		}

		return digits;
	}
};


class Calibration
{
public:
	Calibration(std::string_view const t_text, const Lexer& t_lexer)
	{
		const auto digits = t_lexer.ParseDigits(t_text);

		std::from_chars(digits.data(), std::next(digits.data(), 2), m_num);
	}

	int GetDigit() const
	{
		return m_num;
	}

private:
	int m_num{};
};

int main()
{
	std::ifstream file{ "input.txt" };

	if (!file.is_open())
	{
		return 0;
	}

	std::vector<std::string> lines;
	lines.reserve(1000);

	std::string line{};
	while (std::getline(file, line))
	{
		lines.push_back(std::move(line));
		line = {};
	}

	auto CountCalibrations = [](int t_counter, const Calibration& t_value)
	{
		return t_counter + t_value.GetDigit();
	};

	std::vector<Calibration> calibrations;
	calibrations.reserve(1000);

	for (const auto& fileLine : lines)
	{
		calibrations.push_back(Calibration{ fileLine,DigitsLexer{} });
	}

	const auto sum1 = std::accumulate(std::begin(calibrations), std::end(calibrations), 0, CountCalibrations);

	std::println("First {}", sum1);

	calibrations.clear();

	for (const auto& fileLine : lines)
	{
		calibrations.push_back(Calibration{ fileLine,TextLexer{} });
	}

	const auto sum2 = std::accumulate(std::begin(calibrations), std::end(calibrations), 0, CountCalibrations);

	std::println("Second {}", sum2);




	return 0;
}
