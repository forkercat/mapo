//
// Created by Junhao Wang (@forkercat) on 4/1/24.
//

#include "core/core.h"

#include <entt/entity/registry.hpp>

#include <iostream>

using namespace mapo;

class Test
{
public:
	Test() = default;
};

struct TransformComponent
{
};

struct MeshComponent
{
};

int main()
{
	MP_PRINT("123123!");

	entt::registry m_registry;
	entt::entity e1 = m_registry.create();
	entt::entity e2 = m_registry.create();

	MP_PRINT("%u", e1);
	MP_PRINT("%u", e2);
}
