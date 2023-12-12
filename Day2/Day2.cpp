#include <vector>
#include <string>
#include <fstream>
#include <array>
#include <print>
#include <ranges>
#include <algorithm>

inline constexpr size_t INPUT_RESERVE_SIZE = 100;

std::vector<std::string> GetInput()
{
	std::ifstream file{ "input.txt" };

	if (!file.is_open())
	{
		return {};
	}

	std::vector<std::string> lines;
	lines.reserve(INPUT_RESERVE_SIZE);

	std::string line{};
	while (std::getline(file, line))
	{
		lines.push_back(std::move(line));
		line = {};
	}

	return lines;
}

enum class CubeColor : size_t
{
	RED = 12,
	GREEN = 13,
	BLUE = 14,
};

constexpr CubeColor TextToColor(std::string_view t_text)
{
	if (t_text == "red")
	{
		return CubeColor::RED;
	}
	else if (t_text == "green")
	{
		return CubeColor::GREEN;
	}
	else //if (t_text == "blue")
	{
		return CubeColor::BLUE;
	}
}


class Cube
{
public:
	constexpr Cube(CubeColor t_color, size_t t_count) noexcept
		: m_color(t_color),
		m_count(t_count)
	{
	}

	constexpr size_t GetMax() const noexcept
	{
		return std::to_underlying(m_color);
	}


	CubeColor m_color;
	size_t m_count;
};

class Set
{
public:
	Set(const std::vector<Cube>& t_cubes) 
	{
		std::array<size_t, 3> colorCounters{};
		for (const auto& cube : t_cubes)
		{
			colorCounters[cube.GetMax() - 12] += cube.m_count;

			if (colorCounters[cube.GetMax() - 12] > cube.GetMax())
			{
				m_valid = false;
				break;
			}
		}
	}

	bool m_valid{true};
};

class Game
{
public:
	Game(std::string_view const t_data)
	{
		static constexpr std::string_view begin{"Game "};

		const std::string_view id = t_data.substr(begin.size(), t_data.find(':') - begin.size());
		std::from_chars(id.data(), std::next(id.data(), id.size()), m_id);

		auto const data = std::views::drop(t_data, begin.size() + id.size() + 1);

		for (const auto game : std::views::split(data, ';'))
		{
			std::vector<Cube> cubes; 
			cubes.reserve(4);

			for (auto ball : std::views::split(game, ','))
			{		
				const std::string_view ballInfo{ ball.advance(1) };

				auto const numBegin = ballInfo.data();
				size_t numEndIndex = ballInfo.find(' ');
				auto const numEnd = std::next(numBegin, numEndIndex);

				size_t num{};
				std::from_chars(numBegin, numEnd, num);

				cubes.emplace_back(TextToColor(std::string_view{ ball.advance(numEndIndex + 1) }), num);

			}

			m_sets.emplace_back(Set{ std::move(cubes) });
		}
	}

	bool Valid() const noexcept
	{
		return std::ranges::all_of(m_sets, [](const auto& t_set) { return t_set.m_valid;  });
	}

	size_t GetID() const
	{
		return m_id;
	}


private:
	size_t m_id{};
	std::vector<Set> m_sets;

};

int main()
{	
	const auto lines = GetInput();
	
	std::vector<Game> games;
	games.reserve(INPUT_RESERVE_SIZE);

	size_t sum{};

	for (const auto& line : lines)
	{
		if (const auto& game = games.emplace_back(line);
			game.Valid())
		{
			sum += game.GetID();
		}
	}

	std::println("Part 1 sum {}",sum);

	return 0;
}