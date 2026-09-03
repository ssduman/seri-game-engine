#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <seri/util/Util.h>

TEST_CASE("basic sanity check")
{
	CHECK(1 + 1 == 2);
}

TEST_CASE("Util::RountToInt rounds to nearest integer")
{
	CHECK(seri::Util::RountToInt(2.4f) == 2);
	CHECK(seri::Util::RountToInt(2.5f) == 3);
	CHECK(seri::Util::RountToInt(-2.4f) == -2);
	CHECK(seri::Util::RountToInt(-2.5f) == -3);
	CHECK(seri::Util::RountToInt(0.0f) == 0);
}
