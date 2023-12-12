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

	void AssignCharacter(char const t_character, std::array<char, 2>& t_digits) const
	{
		if (t_digits[0] == 0)
		{
			t_digits[1] = t_digits[0] = t_character;
		}
		else
		{
			t_digits[1] = t_character;
		}
	}

	std::array<char, 2> ParseDigits(std::string_view const t_text) const override
	{
		std::array<char, 2> digits{};

		for (size_t i = 0; i < std::size(t_text); i++)
		{
			if (IsDigit(t_text[i]))
			{
				AssignCharacter(t_text[i], digits);
			}
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
			if (IsDigit(t_text[i]))
			{
				AssignCharacter(t_text[i], digits);
				continue;
			}

			for (size_t j = 0; j < std::size(digitsStr); j++)
			{
				if (t_text.substr(i, digitsStr[j].size()) == digitsStr[j])
				{
					AssignCharacter(static_cast<char>(j) + '1', digits);
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

		m_num = ((digits[0] - '0') * 10) + digits[1] - '0';
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

	calibrations.clear();

	for (const auto& fileLine : lines)
	{
		calibrations.push_back(Calibration{ fileLine,TextLexer{} });
	}

	const auto sum2 = std::accumulate(std::begin(calibrations), std::end(calibrations), 0, CountCalibrations);

	std::println("First: {}\nSecond: {}", sum1, sum2);

	return 0;
}
