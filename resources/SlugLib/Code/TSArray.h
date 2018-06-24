#ifndef TSArray_h
#define TSArray_h


#include <new>


namespace Terathon
{
	inline int32 Max(int32 x, int32 y)
	{
		int32 a = x - y;
		return (x - (a & (a >> 31)));
	}

	template <typename type> class ImmutableArray
	{
		protected:

			int32		elementCount;
			int32		reservedCount;

			type		*arrayPointer;

			ImmutableArray() = default;
			ImmutableArray(const ImmutableArray& array) {}
			~ImmutableArray() = default;

		public:

			operator type *(void)
			{
				return (arrayPointer);
			}

			operator type *(void) const
			{
				return (arrayPointer);
			}

			type *begin(void) const
			{
				return (arrayPointer);
			}

			type *end(void) const
			{
				return (arrayPointer + elementCount);
			}

			bool Empty(void) const
			{
				return (elementCount == 0);
			}

			int32 GetArrayElementCount(void) const
			{
				return (elementCount);
			}

			int32 FindArrayElement(const type& element) const;
	};

	template <typename type> int32 ImmutableArray<type>::FindArrayElement(const type& element) const
	{
		for (machine a = 0; a < elementCount; a++)
		{
			if (arrayPointer[a] == element)
			{
				return (a);
			}
		}

		return (-1);
	}


	template <typename type, int32 baseCount = 0> class Array final : public ImmutableArray<type>
	{
		private:

			using ImmutableArray<type>::elementCount;
			using ImmutableArray<type>::reservedCount;
			using ImmutableArray<type>::arrayPointer;

			char		arrayStorage[baseCount * sizeof(type)];

			void SetReservedCount(int32 count);

		public:

			explicit Array();
			Array(const Array& array);
			Array(Array&& array);
			~Array();

			void ClearArray(void);
			void PurgeArray(void);
			void ReserveArrayElementCount(int32 count);

			void SetArrayElementCount(int32 count);
			void SetArrayElementCount(int32 count, const type& init);
			type *AppendArrayElement(void);

			template <typename T> void AppendArrayElement(T&& element);
			template <typename T> void InsertArrayElement(int32 index, T&& element);

			void RemoveArrayElement(int32 index);
	};


	template <typename type, int32 baseCount> Array<type, baseCount>::Array()
	{
		elementCount = 0;
		reservedCount = baseCount;
		arrayPointer = reinterpret_cast<type *>(arrayStorage);
	}

	template <typename type, int32 baseCount> Array<type, baseCount>::Array(const Array& array)
	{
		elementCount = array.elementCount;
		reservedCount = array.reservedCount;

		if (elementCount > baseCount)
		{
			arrayPointer = reinterpret_cast<type *>(new char[sizeof(type) * reservedCount]);
		}
		else
		{
			arrayPointer = reinterpret_cast<type *>(arrayStorage);
		}

		for (machine a = 0; a < elementCount; a++)
		{
			new(&arrayPointer[a]) type(array.arrayPointer[a]);
		}
	}

	template <typename type, int32 baseCount> Array<type, baseCount>::Array(Array&& array)
	{
		elementCount = array.elementCount;
		reservedCount = array.reservedCount;

		if (elementCount > baseCount)
		{
			arrayPointer = array.arrayPointer;
		}
		else
		{
			arrayPointer = reinterpret_cast<type *>(arrayStorage);

			type *pointer = array.arrayPointer;
			for (machine a = 0; a < elementCount; a++)
			{
				new(&arrayPointer[a]) type(static_cast<type&&>(pointer[a]));
				pointer[a].~type();
			}
		}

		array.elementCount = 0;
		array.reservedCount = baseCount;
		array.arrayPointer = reinterpret_cast<type *>(array.arrayStorage);
	}

	template <typename type, int32 baseCount> Array<type, baseCount>::~Array()
	{
		type *pointer = arrayPointer + elementCount;
		for (machine a = elementCount - 1; a >= 0; a--)
		{
			(--pointer)->~type();
		}

		char *ptr = reinterpret_cast<char *>(arrayPointer);
		if (ptr != arrayStorage)
		{
			delete[] ptr;
		}
	}

	template <typename type, int32 baseCount> void Array<type, baseCount>::ClearArray(void)
	{
		type *pointer = arrayPointer + elementCount;
		for (machine a = elementCount - 1; a >= 0; a--)
		{
			(--pointer)->~type();
		}

		elementCount = 0;
	}

	template <typename type, int32 baseCount> void Array<type, baseCount>::PurgeArray(void)
	{
		type *pointer = arrayPointer + elementCount;
		for (machine a = elementCount - 1; a >= 0; a--)
		{
			(--pointer)->~type();
		}

		char *ptr = reinterpret_cast<char *>(arrayPointer);
		if (ptr != arrayStorage)
		{
			delete[] ptr;
		}

		elementCount = 0;
		reservedCount = baseCount;
		arrayPointer = reinterpret_cast<type *>(arrayStorage);
	}

	template <typename type, int32 baseCount> void Array<type, baseCount>::SetReservedCount(int32 count)
	{
		reservedCount = Max(Max(count, 4), reservedCount + Max((reservedCount / 2 + 3) & ~3, baseCount));
		type *newPointer = reinterpret_cast<type *>(new char[sizeof(type) * reservedCount]);

		type *pointer = arrayPointer;
		for (machine a = 0; a < elementCount; a++)
		{
			new(&newPointer[a]) type(static_cast<type&&>(*pointer));
			pointer->~type();
			pointer++;
		}

		char *ptr = reinterpret_cast<char *>(arrayPointer);
		if (ptr != arrayStorage)
		{
			delete[] ptr;
		}

		arrayPointer = newPointer;
	}

	template <typename type, int32 baseCount> void Array<type, baseCount>::ReserveArrayElementCount(int32 count)
	{
		if (count > reservedCount)
		{
			SetReservedCount(count);
		}
	}

	template <typename type, int32 baseCount> void Array<type, baseCount>::SetArrayElementCount(int32 count)
	{
		if (count > reservedCount)
		{
			SetReservedCount(count);
		}

		if (count > elementCount)
		{
			type *pointer = arrayPointer + (elementCount - 1);
			for (machine a = elementCount; a < count; a++)
			{
				new(++pointer) type;
			}
		}
		else if (count < elementCount)
		{
			type *pointer = arrayPointer + elementCount;
			for (machine a = elementCount - 1; a >= count; a--)
			{
				(--pointer)->~type();
			}
		}

		elementCount = count;
	}

	template <typename type, int32 baseCount> void Array<type, baseCount>::SetArrayElementCount(int32 count, const type& init)
	{
		if (count > reservedCount)
		{
			SetReservedCount(count);
		}

		if (count > elementCount)
		{
			type *pointer = arrayPointer + (elementCount - 1);
			for (machine a = elementCount; a < count; a++)
			{
				new(++pointer) type(init);
			}
		}
		else if (count < elementCount)
		{
			type *pointer = arrayPointer + elementCount;
			for (machine a = elementCount - 1; a >= count; a--)
			{
				(--pointer)->~type();
			}
		}

		elementCount = count;
	}

	template <typename type, int32 baseCount> type *Array<type, baseCount>::AppendArrayElement(void)
	{
		if (elementCount >= reservedCount)
		{
			SetReservedCount(elementCount + 1);
		}

		type *pointer = arrayPointer + elementCount;
		new(pointer) type;

		elementCount++;
		return (pointer);
	}

	template <typename type, int32 baseCount> template <typename T> void Array<type, baseCount>::AppendArrayElement(T&& element)
	{
		if (elementCount >= reservedCount)
		{
			SetReservedCount(elementCount + 1);
		}

		type *pointer = arrayPointer + elementCount;
		new(pointer) type(static_cast<T&&>(element));

		elementCount++;
	}

	template <typename type, int32 baseCount> template <typename T> void Array<type, baseCount>::InsertArrayElement(int32 index, T&& element)
	{
		if (index >= elementCount)
		{
			int32 count = index + 1;
			if (count > reservedCount)
			{
				SetReservedCount(count);
			}

			type *pointer = &arrayPointer[elementCount - 1];
			for (machine a = elementCount; a < index; a++)
			{
				new(++pointer) type;
			}

			new (++pointer) type(static_cast<T&&>(element));
			elementCount = count;
		}
		else
		{
			int32 count = elementCount + 1;
			if (count > reservedCount)
			{
				SetReservedCount(count);
			}

			type *pointer = &arrayPointer[elementCount];
			for (machine a = elementCount; a > index; a--)
			{
				new(pointer) type(static_cast<type&&>(pointer[-1]));
				(--pointer)->~type();
			}

			new (&arrayPointer[index]) type(static_cast<T&&>(element));
			elementCount = count;
		}
	}

	template <typename type, int32 baseCount> void Array<type, baseCount>::RemoveArrayElement(int32 index)
	{
		if (index < elementCount)
		{
			type *pointer = &arrayPointer[index];
			pointer->~type();

			for (machine a = index + 1; a < elementCount; a++)
			{
				new(pointer) type(static_cast<type&&>(pointer[1]));
				(++pointer)->~type();
			}

			elementCount--;
		}
	}


	template <typename type> class Array<type, 0> final : public ImmutableArray<type>
	{
		private:

			using ImmutableArray<type>::elementCount;
			using ImmutableArray<type>::reservedCount;
			using ImmutableArray<type>::arrayPointer;

			void SetReservedCount(int32 count);

		public:

			explicit Array(int32 count = 0);
			Array(const Array& array);
			Array(Array&& array);
			~Array();

			void ClearArray(void);
			void PurgeArray(void);
			void ReserveArrayElementCount(int32 count);

			void SetArrayElementCount(int32 count);
			void SetArrayElementCount(int32 count, const type& init);
			type *AppendArrayElement(void);

			template <typename T> void AppendArrayElement(T&& element);
			template <typename T> void InsertArrayElement(int32 index, T&& element);

			void RemoveArrayElement(int32 index);
	};


	template <typename type> Array<type, 0>::Array(int32 count)
	{
		elementCount = 0;
		reservedCount = count;

		arrayPointer = (count > 0) ? reinterpret_cast<type *>(new char[sizeof(type) * count]) : nullptr;
	}

	template <typename type> Array<type, 0>::Array(const Array& array)
	{
		elementCount = array.elementCount;
		reservedCount = array.reservedCount;

		if (reservedCount > 0)
		{
			arrayPointer = reinterpret_cast<type *>(new char[sizeof(type) * reservedCount]);
			for (machine a = 0; a < elementCount; a++)
			{
				new(&arrayPointer[a]) type(array.arrayPointer[a]);
			}
		}
		else
		{
			arrayPointer = nullptr;
		}
	}

	template <typename type> Array<type, 0>::Array(Array&& array)
	{
		elementCount = array.elementCount;
		reservedCount = array.reservedCount;
		arrayPointer = array.arrayPointer;

		array.elementCount = 0;
		array.reservedCount = 0;
		array.arrayPointer = nullptr;
	}

	template <typename type> Array<type, 0>::~Array()
	{
		type *pointer = arrayPointer + elementCount;
		for (machine a = elementCount - 1; a >= 0; a--)
		{
			(--pointer)->~type();
		}

		delete[] reinterpret_cast<char *>(arrayPointer);
	}

	template <typename type> void Array<type, 0>::ClearArray(void)
	{
		type *pointer = arrayPointer + elementCount;
		for (machine a = elementCount - 1; a >= 0; a--)
		{
			(--pointer)->~type();
		}

		elementCount = 0;
	}

	template <typename type> void Array<type, 0>::PurgeArray(void)
	{
		type *pointer = arrayPointer + elementCount;
		for (machine a = elementCount - 1; a >= 0; a--)
		{
			(--pointer)->~type();
		}

		delete[] reinterpret_cast<char *>(arrayPointer);

		elementCount = 0;
		reservedCount = 0;
		arrayPointer = nullptr;
	}

	template <typename type> void Array<type, 0>::SetReservedCount(int32 count)
	{
		reservedCount = Max(Max(count, 4), reservedCount + Max((reservedCount / 2 + 3) & ~3, 4));
		type *newPointer = reinterpret_cast<type *>(new char[sizeof(type) * reservedCount]);

		type *pointer = arrayPointer;
		if (pointer)
		{
			for (machine a = 0; a < elementCount; a++)
			{
				new(&newPointer[a]) type(static_cast<type&&>(*pointer));
				pointer->~type();
				pointer++;
			}

			delete[] reinterpret_cast<char *>(arrayPointer);
		}

		arrayPointer = newPointer;
	}

	template <typename type> void Array<type, 0>::ReserveArrayElementCount(int32 count)
	{
		if (count > reservedCount)
		{
			SetReservedCount(count);
		}
	}

	template <typename type> void Array<type, 0>::SetArrayElementCount(int32 count)
	{
		if (count > reservedCount)
		{
			SetReservedCount(count);
		}

		if (count > elementCount)
		{
			type *pointer = arrayPointer + (elementCount - 1);
			for (machine a = elementCount; a < count; a++)
			{
				new(++pointer) type;
			}
		}
		else if (count < elementCount)
		{
			type *pointer = arrayPointer + elementCount;
			for (machine a = elementCount - 1; a >= count; a--)
			{
				(--pointer)->~type();
			}
		}

		elementCount = count;
	}

	template <typename type> void Array<type, 0>::SetArrayElementCount(int32 count, const type& init)
	{
		if (count > reservedCount)
		{
			SetReservedCount(count);
		}

		if (count > elementCount)
		{
			type *pointer = arrayPointer + (elementCount - 1);
			for (machine a = elementCount; a < count; a++)
			{
				new(++pointer) type(init);
			}
		}
		else if (count < elementCount)
		{
			type *pointer = arrayPointer + elementCount;
			for (machine a = elementCount - 1; a >= count; a--)
			{
				(--pointer)->~type();
			}
		}

		elementCount = count;
	}

	template <typename type> type *Array<type, 0>::AppendArrayElement(void)
	{
		if (elementCount >= reservedCount)
		{
			SetReservedCount(elementCount + 1);
		}

		type *pointer = arrayPointer + elementCount;
		new(pointer) type;

		elementCount++;
		return (pointer);
	}

	template <typename type> template <typename T> void Array<type, 0>::AppendArrayElement(T&& element)
	{
		if (elementCount >= reservedCount)
		{
			SetReservedCount(elementCount + 1);
		}

		type *pointer = arrayPointer + elementCount;
		new(pointer) type(static_cast<T&&>(element));

		elementCount++;
	}

	template <typename type> template <typename T> void Array<type, 0>::InsertArrayElement(int32 index, T&& element)
	{
		if (index >= elementCount)
		{
			int32 count = index + 1;
			if (count > reservedCount)
			{
				SetReservedCount(count);
			}

			type *pointer = &arrayPointer[elementCount - 1];
			for (machine a = elementCount; a < index; a++)
			{
				new(++pointer) type;
			}

			new (++pointer) type(static_cast<T&&>(element));
			elementCount = count;
		}
		else
		{
			int32 count = elementCount + 1;
			if (count > reservedCount)
			{
				SetReservedCount(count);
			}

			type *pointer = &arrayPointer[elementCount];
			for (machine a = elementCount; a > index; a--)
			{
				new(pointer) type(static_cast<type&&>(pointer[-1]));
				(--pointer)->~type();
			}

			new (&arrayPointer[index]) type(static_cast<T&&>(element));
			elementCount = count;
		}
	}

	template <typename type> void Array<type, 0>::RemoveArrayElement(int32 index)
	{
		if (index < elementCount)
		{
			type *pointer = &arrayPointer[index];
			pointer->~type();

			for (machine a = index + 1; a < elementCount; a++)
			{
				new(pointer) type(static_cast<type&&>(pointer[1]));
				(++pointer)->~type();
			}

			elementCount--;
		}
	}
}


#endif
