#include <vector>
#include <string>
#include <fstream>
#include <array>
#include <print>
#include <ranges>
#include <algorithm>
#include <numeric>

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

enum class CubeColor : int
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
	constexpr Cube(CubeColor t_color, int t_count) noexcept
		: m_color(t_color),
		m_count(t_count)
	{
	}

	constexpr int GetMax() const noexcept
	{
		return std::to_underlying(m_color);
	}


	CubeColor m_color;
	int m_count;
};

class Set
{
public:
	Set(std::vector<Cube> t_cubes)
	{
		for (const auto& cube : t_cubes)
		{
			const auto index = static_cast<size_t>(cube.GetMax() - 12);

			m_colorCounters[index] += cube.m_count;

			if (m_colorCounters[index] > cube.GetMax())
			{
				m_valid = false;
			}
		}
	}

	std::array<int, 3> m_colorCounters{};
	bool m_valid{ true };
};

class Game
{
public:
	Game(std::string_view const t_data)
	{
		static constexpr std::string_view begin{ "Game " };

		const std::string_view id = t_data.substr(begin.size(), t_data.find(':') - begin.size());
		std::from_chars(id.data(), std::next(id.data(), static_cast<std::ptrdiff_t>(id.size())), m_id);

		auto const data = std::views::drop(t_data, static_cast<std::ptrdiff_t>(begin.size() + id.size() + 1));

		m_sets.reserve(3);

		for (const auto game : std::views::split(data, ';'))
		{
			std::vector<Cube> cubes;
			cubes.reserve(4);

			for (const auto ball : std::views::split(game, ','))
			{
				const std::string_view ballInfo{ ball | std::views::drop(1) };
				
				const auto numEndIndex = ballInfo.find(' ');

				int num{};
				(void)std::from_chars(ballInfo.data(), std::next(ballInfo.data(), static_cast<std::ptrdiff_t>(numEndIndex)), num);
				cubes.emplace_back(TextToColor(ballInfo.substr(numEndIndex + 1)), num);
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

	std::array<int,3> GetMinimalCubesCount() const
	{
		std::array<int, 3> count{};

		for (const auto& set : m_sets)
		{
			for (size_t i = 0; i < std::size(count); i++)
			{
				count[i] = std::max(count[i], set.m_colorCounters[i]);
			}
		}

		return count;
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

	size_t validGamesSum{};
	size_t powerSum{};
	for (const auto& line : lines)
	{
		const auto& game = games.emplace_back(line);
		if (game.Valid())
		{
			validGamesSum += game.GetID();
		}

		const auto cubesCount = game.GetMinimalCubesCount();

		powerSum += std::accumulate(std::begin(cubesCount), std::end(cubesCount), 1, [](auto t_left, auto t_right)
		{
			return t_left * t_right;
		});

	}

	std::println("Part 1 sum of valid games {}\nPart 2 sum of powers {}", validGamesSum, powerSum);

	return 0;
}