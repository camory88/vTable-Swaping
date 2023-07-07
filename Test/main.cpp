#include <iostream>
#include <Windows.h>
bool IsKeyDown(int virtualKeyCode)
{
	// Check the high-order bit to determine if the key is currently down
	return (GetAsyncKeyState(virtualKeyCode) & 0x8000) != 0;
}

class Entity
{
public:
	virtual void Talk() = 0;
	virtual void healthCheack() = 0;

	int health;
};

class Player : public Entity
{
public:
	virtual void Talk() override {
		printf("Hello from player 0x%p\n",this);
	}
	virtual void healthCheack() override {
		
		health = 100;
		printf("Health = %i\n",health);
	}
};

void (*Talk_orig)(void*);
void Talk_hook(void* _this)
{
	printf("Talked HOOKED 0x%p\n", _this);
	Talk_orig(_this);
}

int main()
{
	//EntBase
	Player* player = new Player;
	//Get Vtable from EntBase
	uint64_t* Vtable = *(uint64_t**)(player + 0x0);

	printf("Vtable = 0x%11x\n", Vtable);

	//get the function in the Vtable
	uint64_t function_address = Vtable[0];

	printf("Fiunction = 0x%11x\n", function_address);

	//declare function and prams
	void(*Talk_fn)(void*) = 0;
	Talk_fn = (decltype(Talk_fn))function_address;

	//call Vtable func
	Talk_fn(player);


	
	DWORD old;//value for old func
	VirtualProtect(Vtable, sizeof(void*), PAGE_EXECUTE_READWRITE, &old);//Protects memory of func
	Talk_orig = (decltype(Talk_orig))Vtable[0]; //writes old func to DWORD old 
	Vtable[0] = (uint64_t)&Talk_hook;//re-writes old func to are func and calles old func again
	VirtualProtect(Vtable, sizeof(void*), old, &old);// Protects memory of new hooked func

	//declare function with it hooked
	Talk_fn = (decltype(Talk_fn))Vtable[0];
	//call are Vtable func
	Talk_fn(player);

	while (!IsKeyDown(VK_DELETE))
	{
		Sleep(100);
	}
	return 0;

}