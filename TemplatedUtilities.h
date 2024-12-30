#pragma once
#include <algorithm>
#include <utility>
#include <type_traits>
#include <vector>
#include <set>
#include <array>
#include <unordered_map>
#include <bit>
#include <optional>
#include <cassert>

//////////////////////////////////////////////////////////////////
// Utility methods
//////////////////////////////////////////////////////////////////
template <typename InputIterator1, typename InputIterator2>
[[nodiscard]] bool set_intersects(InputIterator1 first1, InputIterator1 last1,
					InputIterator2 first2, InputIterator2 last2)
{
	while (first1 != last1 && first2 != last2)
	{
		if (*first1 < *first2) ++first1;
		else if (*first2 < *first1) ++first2;
		else return true;
	}
	return false;
}

template <typename InputIterator1, typename InputIterator2>
[[nodiscard]] size_t set_intersect_size(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2)
{
	size_t result = 0;
	while (first1 != last1 && first2 != last2)
	{
		if (auto comp = *first1 <=> *first2;
			comp < 0) ++first1;
		else if (comp > 0) ++first2;
		else { ++result; ++first1; ++first2; }
	}
	return result;
}

template <typename T>
constexpr void GrowToEncompass(T& min, T& max, T val)
{
	min = std::min(min, val);
	max = std::max(max, val);
}

template <typename T>
[[nodiscard]] constexpr bool Within(T min, T val, T max)
{
	return min <= val && val <= max;
}

template <typename T, typename Op, typename R = std::iterator_traits<T>::reference>
[[nodiscard]] std::optional<std::pair<R, R>> TwoPointersSearch(T low, T high, const Op& ThreeWayPred)
{
	if (low == high)
		return {};
	--high;
	while (low != high)
	{
		if (auto check = ThreeWayPred(*low, *high);
			check == 0)
			return std::make_pair(std::ref(*low), std::ref(*high));
		else if (check < 0)
			++low;
		else if (check > 0)
			--high;
		else
		{
			assert(false && "Invalid ordering");
			break;
		}
	}
	return {};
}

///////////////////////////////////////////////////////////////////
// Recursive lambda support
///////////////////////////////////////////////////////////////////
template <typename T>
class y_combinator {
	T lambda;
public:
	constexpr y_combinator(T&& t) : lambda(std::forward<T>(t)) {}
	template <typename...Args>
	constexpr decltype(auto) operator()(Args&&...args) const { return lambda(std::move(*this), std::forward<Args>(args)...); }
};

///////////////////////////////////////////////////////////////////
// Coordinate systems
///////////////////////////////////////////////////////////////////
template <typename T>
constexpr bool IsEqual(T a, T b)
{
	if constexpr (std::is_floating_point_v<T>)
		return (a < b ? b - a : a - b) < static_cast<T>(0.000001);
	else
		return a == b;
}

template<typename T>
struct Coord
{
	constexpr bool operator <  (const Coord& p) const { if (x < p.x) return true; else if (p.x < x) return false; else return y < p.y; }
	constexpr Coord operator + (const Coord& p) const { return Coord(x + p.x, y + p.y); }
	constexpr Coord& operator+=(const Coord& p) { x += p.x; y += p.y; return *this; }
	constexpr Coord operator - (const Coord& p) const { return Coord(x - p.x, y - p.y); }
	constexpr Coord& operator-=(const Coord& p) { x -= p.x; y -= p.y; return *this; }
	constexpr Coord operator * (T scalar) const { return Coord(x * scalar, y * scalar); }
	constexpr Coord& operator*=(T scalar) { x *= scalar; y *= scalar; return *this; }
	constexpr Coord operator % (const Coord& p) const  { return Coord(x % p.x, y % p.y); }
	constexpr Coord& operator%=(const Coord& p) { x %= p.x; y %= p.y; return *this; }
	constexpr bool operator==(const Coord& p) const { return IsEqual(x, p.x) && IsEqual(y, p.y); }
	constexpr bool operator!=(const Coord& p) const { return !IsEqual(x, p.x) || !IsEqual(y, p.y); }
	// Warning, much slower
	friend auto operator <=>(const Coord&, const Coord&) = default;

	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	bool Within(const U& grid) const { return x >= 0 && y >= 0 && x < grid.size() && y < grid[x].size(); }
	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	auto& GetAt(U& grid) const { return grid[x][y]; }
	template<typename U, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	const auto& GetAt(const U& grid) const { return grid[x][y]; }

	T x = {}, y = {};
};
using Point = Coord<int>;
constexpr std::array<Point, 4> directions = { Point(1, 0), Point(0, 1), Point(-1, 0), Point(0,-1) };
constexpr std::array<char, 4> coords = { 'N', 'E', 'S', 'W' };

template<typename T>
T ManhattanDistance(const Coord<T>& c1, const Coord<T>& c2)
{
	if constexpr (std::is_signed_v<T>)
		return std::abs(c1.x - c2.x) + std::abs(c1.y - c2.y);
	else
		return (c2.x < c1.x ? c1.x - c2.x : c2.x - c1.x) + (c2.y < c1.y ? c1.y - c2.y : c2.y - c1.y);
}

template<typename T>
struct Coord3D
{
	constexpr Coord3D(T X = 0, T Y = 0, T Z = 0) : x(X), y(Y), z(Z) {}
	constexpr Coord3D(const std::array<T, 3>& a) : x(a[0]), y(a[1]), z(a[2]) {}
	constexpr bool operator <  (const Coord3D& p) const { if (x < p.x) return true; if (p.x < x) return false; if (y < p.y) return true; if (p.y < y) return false; return z < p.z; }
	constexpr Coord3D operator + (const Coord3D& p) const { return Coord3D(x + p.x, y + p.y, z + p.z); }
	constexpr Coord3D& operator+=(const Coord3D& p) { x += p.x; y += p.y; z += p.z; return *this; }
	constexpr Coord3D operator - (const Coord3D& p) const { return Coord3D(x - p.x, y - p.y, z - p.z); };
	constexpr Coord3D& operator-=(const Coord3D& p) { x -= p.x; y -= p.y; z -= p.z; return *this; };
	constexpr Coord3D operator * (T scalar) const { return Coord3D(x * scalar, y * scalar, z * scalar); }
	constexpr Coord3D& operator*=(T scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
	constexpr bool operator==(const Coord3D& p) const { return IsEqual(x, p.x) && IsEqual(y, p.y) && IsEqual(z, p.z); }
	constexpr bool operator!=(const Coord3D& p) const { return !IsEqual(x, p.x) || !IsEqual(y, p.y) || !IsEqual(z, p.z); }
	friend auto operator <=>(const Coord3D&, const Coord3D&) = default;
	constexpr std::array<T, 3> AsArray() const { return { x, y, z }; }
	T x, y, z;
};

using Point3D = Coord3D<int>;
constexpr std::array<Point3D, 4> flatDirections = { Point3D(1, 0, 0), Point3D(0, 1, 0), Point3D(-1, 0, 0), Point3D(0, -1, 0) };
constexpr std::array<Point3D, 6> AllDirections = { Point3D(1, 0, 0), Point3D(0, 1, 0), Point(0, 0, 1), Point3D(-1, 0, 0), Point3D(0, -1, 0), Point(0, 0, -1) };

template<typename T>
struct Coord4D
{
	constexpr Coord4D(T X = 0, T Y = 0, T Z = 0, T W = 0) : x(X), y(Y), z(Z), w(W) {}
	constexpr bool operator <(const Coord4D& p) const { if (x < p.x) return true; if (p.x < x) return false; if (y < p.y) return true; if (p.y < y) return false; if (z < p.z) return true; if (p.z < z) return false; return w < p.w; }
	constexpr bool operator==(const Coord4D& p) const { return IsEqual(x, p.x) && IsEqual(y, p.y) && IsEqual(z, p.z) && IsEqual(w, p.w); }
	constexpr bool operator!=(const Coord4D& p) const { return !IsEqual(x, p.x) || !IsEqual(y, p.y) || !IsEqual(z, p.z) || !IsEqual(w, p.w); }
	friend auto operator <=>(const Coord&, const Coord&) = default;
	T x, y, z, w;
};

using Point4D = Coord4D<int>;

template<typename T>
[[nodiscard]] constexpr Coord<T> ReverseDirection(const Coord<T>& p)
{
	return Coord<T>(-p.x, -p.y);
}

template <typename T>
struct Boundaries
{
	constexpr Boundaries() : minX(0), minY(0), maxX(0), maxY(0) {}
	constexpr Boundaries(const Coord<T>& c) : minX(c.x), minY(c.y), maxX(c.x), maxY(c.y) {}
	constexpr const Boundaries& operator+=(const Boundaries& b) {
		minX = std::min(minX, b.minX);
		minY = std::min(minY, b.minY);
		maxX = std::max(maxX, b.maxX);
		maxY = std::max(maxY, b.maxY);
		return *this;
	}
	constexpr const Boundaries& operator+=(const Coord<T>& p) { GrowToEncompass(minX, maxX, p.x); GrowToEncompass(minY, maxY, p.y); return *this; }
	constexpr void Translate(const Coord<T>& p) { minX += p.x; maxX += p.x; minY += p.y; maxY += p.y; }
	constexpr bool Inside(const Coord<T>& p) const { return Within(minX, p.x, maxX) && Within(minY, p.y, maxY); }
	T minX, minY, maxX, maxY;
};

using Bounds = Boundaries<int>;


/////////////////////////////////////////////////////////////////
// Integer and modulo mathematics
/////////////////////////////////////////////////////////////////
// From : https://stackoverflow.com/questions/4930307/fastest-way-to-get-the-integer-part-of-sqrtn
// Modified to C++20 code and accept signed integers
template <std::integral T>
[[nodiscard]] constexpr T sqrti(T n)
{
	if (n < 2) return n;
	unsigned char shift;
	if constexpr (std::is_unsigned_v<T>) shift = std::bit_width(n);
	else
	{
		T work = n;
		do
		{
			++shift;
			work >>= 1;
		} while (work);
	}
	shift += shift & 1;
	T result = 0;
	do
	{
		shift -= 2;
		result = (result << 1) | 1;
		result ^= result * result > (n >> shift);
	} while (shift);

	return result;
}

template <std::integral T>
[[nodiscard]] constexpr T Xor0ToN(T n)
{
	if constexpr (std::is_signed_v<T>)
		if (n < 0)
			return (((n & 2) - 1) & ~((((n >> 1) ^ n) & 1) - 1)) + (~((n & 1) - 1) & n);
	return (n & (n << 1 & 2) - 1) ^ (n >> 1 & 1);
}

template <std::integral T>
std::vector<T> SieveOfEratosthenes(T maxNum)
{
	std::vector<char> test(maxNum, 1);
	test[1] = test[0] = 0;
	//T index = 1, stop = static_cast<T>(std::ceil(sqrt(maxNum)));
	T index = 1, stop = sqrti(maxNum);
	while (++index <= stop)
		if (test[index])
			for (T i = index * index, increase = (index == 2ull ? 2ull : 2ull * index); i < maxNum; i += increase)
				test[i] = 0;
	
	std::vector<T> primes;
	for (T i = 0; i < maxNum; ++i)
		if (test[i])
			primes.push_back(i);

	return primes;
}


// From https://www.geeksforgeeks.org/multiply-large-integers-under-large-modulo/
template <std::integral T>
[[nodiscard]] constexpr T ModuloMul(T a, T b, T mod)
{
	T result = 0;
	a %= mod;
	while (b)
	{
		if (b & 1)
			result = (result + a) % mod;
		a = (2 * a) % mod;
		b /= 2;
	}
	return result;
}

// From https://stackoverflow.com/questions/8496182/calculating-powa-b-mod-n
// Primality testing approach
// Modified to avoid multiplication overflow
template <std::integral T>
[[nodiscard]] constexpr T ModuloExp(T base, T exp, T mod)
{
	T result = 1;
	while (exp)
	{
		if (exp & 1)
			result = ModuloMul(result, base, mod);
		base = ModuloMul(base, base, mod);
		exp /= 2;
	}
	return result % mod;
}

// From https://www.geeksforgeeks.org/discrete-logarithm-find-integer-k-ak-congruent-modulo-b/
template<std::integral T>
[[nodiscard]] T ModuloLog(T a, T b, const T m)
{
	T n = sqrti(m) + 1;
	std::unordered_map<T, T> value;
	for (T i = n; i >= 1; --i)
		value[ModuloExp(a, i * n, m)] = i;
	for (T j = 0; j < n; ++j)
		if (T cur = ModuloMul(ModuloExp(a, j, m), b, m);
			value[cur])
			if (T ans = value[cur] * n - j;
				ans < m)
				return ans;
	return static_cast<T>(-1);
}


// From https://www.rookieslab.com/posts/how-to-find-multiplicative-inverse-of-a-number-modulo-m-in-python-cpp
template <std::integral T>
[[nodiscard]] constexpr T ModuloInvMul(T A, T M) {
	// Assumes that M is a prime number
	// Returns multiplicative modulo inverse of A under M
	return ModuloExp(A, M - 2, M);
}

template<std::integral T>
[[nodiscard]] constexpr std::array<T, 3> ExtendedEuclidGCD(T a, T b) {
	// Returns a vector `result` of size 3 where:
	// Referring to the equation ax + by = gcd(a, b)
	//     result[0] is gcd(a, b)
	//     result[1] is x
	//     result[2] is y 

	T s = 0, old_s = 1;
	T t = 1, old_t = 0;
	T r = b, old_r = a;

	while (r != 0) {
		T quotient = old_r / r;
		// We are overriding the value of r, before that we store it's current
		// value in temp variable, later we assign it to old_r
		T temp = r;
		r = old_r - quotient * r;
		old_r = temp;

		// We treat s and t in the same manner we treated r
		temp = s;
		s = old_s - quotient * s;
		old_s = temp;

		temp = t;
		t = old_t - quotient * t;
		old_t = temp;
	}
	std::array<T, 3> result{ old_r, old_s, old_t };
	return result;
}

template <std::integral T>
[[nodiscard]] constexpr T ModuloInvMul_NonPrime(T A, T M) {
	// Assumes that A and M are co-prime
	// Returns multiplicative modulo inverse of A under M

	// Find gcd using Extended Euclid's Algorithm
	std::array<T, 3> v = ExtendedEuclidGCD(A, M);
	//i64 gcd = v[0];
	//i64 x = v[1];
	//i64 y = v[2]; // We don't really need this though

	// In case x is negative, we handle it by adding extra M
	// Because we know that multiplicative inverse of A in range M lies
	// in the range [0, M-1]
	return v[1] < 0 ? v[1] + M : v[1];
}

///////////////////////////////////////////////////////////////////////////////
// Heap utilities
///////////////////////////////////////////////////////////////////////////////
template <typename T>
void PushHeap(T& container, const typename T::value_type& val)
{
	container.push_back(val);
	std::push_heap(container.begin(), container.end());
}

template <typename T, typename Predicate>
void PushHeap(T& container, const typename T::value_type& val, const Predicate& pred)
{
	container.push_back(val);
	std::push_heap(container.begin(), container.end(), pred);
}

template <typename T>
typename T::value_type PopHeap(T& container)
{
	typename T::value_type result = container.front();
	std::pop_heap(container.begin(), container.end());
	container.pop_back();
	return result;
}

template <typename T, typename Predicate>
typename T::value_type PopHeap(T& container, const Predicate& pred)
{
	typename T::value_type result = container.front();
	std::pop_heap(container.begin(), container.end(), pred);
	container.pop_back();
	return result;
}

template <typename T, typename Predicate, typename... args>
void EmplaceHeap(T& container, const Predicate& pred, args&&... a)
{
	container.emplace_back(a...);
	std::push_heap(container.begin(), container.end(), pred);
}

///////////////////////////////////////////////////////////////////
// Typical Tasks
///////////////////////////////////////////////////////////////////
template<typename T, u64 MinSize>
class CycleFinder
{
public:
	std::optional<T> TryFindAt(const T& value, u64 target)
	{
		values.push_back(value);
		if (auto iter = occurrences.find(value);
			iter == occurrences.cend())
			occurrences[value] = { values.size() - 1 };
		else
		{
			auto& v = iter->second;
			v.push_back(values.size() - 1);
			for (int j = v.size() - 2; j > 0; --j)
				if (u64 cycle = v.back() - v[j];
					cycle >= MinSize && std::find(v.cbegin(), v.cbegin() + j, v.back() - 2 * cycle) != v.cbegin() + j)
			{
				bool valid = true;
				for (int k = 0; k <= cycle && valid; ++k)
					valid = (values[v.back() - k] == values[v[j] - k]);

				if (valid)
				{
					u64 diff = (target - values.size()) % cycle;
					return *(values.cend() - 1 - (cycle - diff));
				}
			}
		}
		return {};
	}

private:
	std::map<T, std::vector<u64>> occurrences = {};
	std::vector<T> values = {};
};


///////////////////////////////////////////////////////////////////////////////
// A* base implementation
///////////////////////////////////////////////////////////////////////////////

template <typename Info_t, typename Cost_t, bool Unique>
struct AStarInfo
{
	constexpr AStarInfo(const Info_t& i, Cost_t c) : info(i), cost_plus_Eval{}, cost(c) {}
	constexpr AStarInfo(const Info_t& i, Cost_t c, Cost_t eval) : info(i), cost_plus_Eval(c + eval), cost(c) {}

	constexpr bool operator > (const AStarInfo& right) const { return cost_plus_Eval > right.cost_plus_Eval || (cost_plus_Eval == right.cost_plus_Eval && info > right.info); }
	constexpr bool RepeatSetCheck(const AStarInfo& right) const
	{
		if constexpr (Unique) return info < right.info;
		else return info < right.info || (info == right.info && cost < right.cost);
	}
	Info_t info;
	Cost_t cost_plus_Eval;
	Cost_t cost;
};

template <typename ASI, typename Expand, typename Estimate, typename TargetEquality>
class AStar
{
public:
	using Cost_t = decltype(ASI::cost);

	AStar(const ASI& start, const Expand& ex, const Estimate& es, const TargetEquality& eq) : expand(ex), estimate(es), equality(eq)
	{
		Initialize(std::vector<ASI>{start});
	}

	AStar(const std::vector<ASI>& start, const Expand& ex, const Estimate& es, const TargetEquality& eq) : expand(ex), estimate(es), equality(eq)
	{
		Initialize(start);
	}

	void Initialize(const ASI& start)
	{
		Initialize(std::vector<ASI>{start});
	}

	void Initialize(const std::vector<ASI>& start)
	{
		std::priority_queue<ASI, std::vector<ASI>, std::greater<ASI>> empty;
		std::swap(toEvaluate, empty);
		repeatGuard.clear();
		for (const ASI& asi : start)
			toEvaluate.push(asi);
	}


	std::optional<Cost_t> Evaluate()
	{
		while (!toEvaluate.empty())
		{
			ASI current = toEvaluate.top();
			if (equality(current))
				return current.cost;

			toEvaluate.pop();
			if (repeatGuard.insert(current).second)
			{
				auto toAdd = expand(current);
				for (ASI& a : toAdd)
				{
					a.cost_plus_Eval = a.cost + estimate(a);
					toEvaluate.push(a);
				}
			}
		}
		return {};
	}

protected:
	struct Comparator { bool operator()(const ASI& l, const ASI& r) const { return l.RepeatSetCheck(r); } };
	std::set<ASI, Comparator> repeatGuard;
	std::priority_queue<ASI, std::vector<ASI>, std::greater<ASI>> toEvaluate;
	Expand expand;
	Estimate estimate;
	TargetEquality equality;
};



