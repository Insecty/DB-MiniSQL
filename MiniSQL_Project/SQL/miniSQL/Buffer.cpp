#include "Buffer.h"
#include <limits.h>
#include <string.h>
#include <iostream>

Buffer bufferMan;

Buffer::Buffer()
{
	for (int i = 0; i<BUFFERBLOCKNUM; i++)
	{
		Valid[i] = false;
	}
}


int Buffer::GetLRU()
{
	for (int i = 0; i<BUFFERBLOCKNUM; i++)
	{
		if (!Valid[i]) return i;
	}
	time_t temp = INT_MAX;
	int j = 0;
	for (int i = 0; i<BUFFERBLOCKNUM; i++)
	{
		if (Cache[i].Time<temp)
		{
			temp = Cache[i].Time;
			j = i;
		}
	}
	return j;
}

int Buffer::Index(int BlockID)
{
	for (int i = 0; i<BUFFERBLOCKNUM; i++)
	{
		if (Cache[i].ID == BlockID) return i;
	}
	return -1;
}

bool Buffer::Hit(int BlockID)
{
	for (int i = 0; i<BUFFERBLOCKNUM; i++)
	{
		if (Valid[i] && Cache[i].ID == BlockID) return true;
	}
	return false;
}

int Buffer::Insert(string Name, int RecordSize, char* R)
{
	TableFile T = TableFile::ReadTableFromDisk(Name);

	int BlockID = T.GetNotFullBlock(RecordSize);

	if (Hit(BlockID))  //ÃüÖÐ
	{
		int index = Index(BlockID);
		Cache[index].InsertRecord(R);
		T.AddBlockToTable(Cache[index]);
		T.WriteTableToDisk();
		Cache[index].WriteBlockToDisk();
		time(&Cache[BlockID].Time);
	}
	else  //È±Ê§
	{
		Block temp = Block::ReadBlockFromDisk(BlockID);
		int Replace = GetLRU();
		Cache[Replace] = temp;
		Valid[Replace] = true;
		Cache[Replace].InsertRecord(R);;
		T.AddBlockToTable(Cache[Replace]);
		T.WriteTableToDisk();
		Cache[Replace].WriteBlockToDisk();
		time(&Cache[Replace].Time);
	}
	return BlockID;
}



TheSelectResult Buffer::Delete(string Name, int BlockID[], int Offset, int Size, char* Key, eColType2 type, eSign op)
{

	TableFile T = TableFile::ReadTableFromDisk(Name);
	if (op == eSignDEFAULT) //delete *
	{
		for (int i = 1; i <= MAXBLOCKNUM; i++)
		{
			if (T.TableBlock[i])
			{
				T.RemoveBlockFromTable(i);
				T.WriteTableToDisk();
				for (int j = 0; j<BUFFERBLOCKNUM; j++)
				{
					if (Cache[j].ID == i&&Valid[j]) Valid[j] = false;
				}
			}
		}
		TheSelectResult R(0);
		return R;
	}
	else
	{
		if (BlockID[0] == 0)
		{
			TheSelectResult R(0);
			return R;
		}
		Block Temp = Block::ReadBlockFromDisk(BlockID[1]);
		TheSelectResult Result(Temp.RecordSize);
		for (int i = 0; i<BlockID[0]; i++)
		{
			if (Hit(BlockID[i])) //ÃüÖÐ
			{
				int index = Index(BlockID[i]);
				switch (type)
				{
				case Int:
					switch (op)
					{
					case BIGGER:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (*((int*)(Cache[index].Data + j*Cache[index].RecordSize + Offset))>(*(int*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case SMALLER:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (*((int*)(Cache[index].Data + j*Cache[index].RecordSize + Offset))<(*(int*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case EQUAL:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (*((int*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) == (*(int*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case EBIGGER:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (*((int*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) >= (*(int*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case ESMALLER:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (*((int*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) <= (*(int*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case NEQUAL:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (*((int*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) != (*(int*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					default:
						break;
					}
					break;




				case Float:
					switch (op)
					{
					case BIGGER:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (*((float*)(Cache[index].Data + j*Cache[index].RecordSize + Offset))>(*(float*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case SMALLER:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (*((float*)(Cache[index].Data + j*Cache[index].RecordSize + Offset))<(*(float*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case EQUAL:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (*((float*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) == (*(float*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case EBIGGER:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (*((float*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) >= (*(float*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case ESMALLER:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (*((float*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) <= (*(float*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case NEQUAL:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (*((float*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) != (*(float*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					default:
						break;
					}
					break;


				case Char:
					switch (op)
					{
					case BIGGER:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (strncmp(Cache[index].Data + j*Cache[index].RecordSize + Offset, Key, (size_t)Size)>0)
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case SMALLER:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (strncmp(Cache[index].Data + j*Cache[index].RecordSize + Offset, Key, (size_t)Size)<0)
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case EQUAL:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (strncmp(Cache[index].Data + j*Cache[index].RecordSize + Offset, Key, (size_t)Size) == 0)
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case EBIGGER:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (strncmp(Cache[index].Data + j*Cache[index].RecordSize + Offset, Key, (size_t)Size) >= 0)
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case ESMALLER:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (strncmp(Cache[index].Data + j*Cache[index].RecordSize + Offset, Key, (size_t)Size) <= 0)
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					case NEQUAL:
						for (int j = 0; j<Cache[index].RecordNum; j++)
						{
							if (strncmp(Cache[index].Data + j*Cache[index].RecordSize + Offset, Key, (size_t)Size) != 0)
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[index].RemoveRecord(j);
								j--;
								if (Cache[index].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[index].ID);
									T.WriteTableToDisk();
									Valid[index] = false;
								}
								else
								{
									Cache[index].WriteBlockToDisk();
								}
							}
						}
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
				time(&Cache[index].Time);
			}

			else  //È±Ê§
			{
				Block temp = Block::ReadBlockFromDisk(BlockID[i]);
				int Replace = GetLRU();
				Cache[Replace] = temp;
				switch (type)
				{
				case Int:
					switch (op)
					{
					case BIGGER:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (*((int*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset))>(*(int*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case SMALLER:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (*((int*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset))<(*(int*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case EQUAL:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (*((int*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) == (*(int*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case EBIGGER:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (*((int*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) >= (*(int*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case ESMALLER:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (*((int*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) <= (*(int*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case NEQUAL:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (*((int*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) != (*(int*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					default:
						break;
					}
					break;




				case Float:
					switch (op)
					{
					case BIGGER:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (*((float*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset))>(*(float*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case SMALLER:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (*((float*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset))<(*(float*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case EQUAL:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (*((float*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) == (*(float*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case EBIGGER:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (*((float*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) >= (*(float*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case ESMALLER:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (*((float*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) <= (*(float*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case NEQUAL:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (*((float*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) != (*(float*)Key))
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					default:
						break;
					}
					break;


				case Char:
					switch (op)
					{
					case BIGGER:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (strncmp(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset, Key, (size_t)Size)>0)
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case SMALLER:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (strncmp(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset, Key, (size_t)Size)<0)
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case EQUAL:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (strncmp(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset, Key, (size_t)Size) == 0)
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case EBIGGER:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (strncmp(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset, Key, (size_t)Size) >= 0)
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case ESMALLER:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (strncmp(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset, Key, (size_t)Size) <= 0)
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					case NEQUAL:
						for (int j = 0; j<Cache[Replace].RecordNum; j++)
						{
							if (strncmp(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset, Key, (size_t)Size) != 0)
							{
								memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
								Result.RecordNum++;
								Cache[Replace].RemoveRecord(j);
								j--;
								if (Cache[Replace].RecordNum == 0)
								{
									T.RemoveBlockFromTable(Cache[Replace].ID);
									T.WriteTableToDisk();
									Valid[Replace] = false;
								}
								else
								{
									Cache[Replace].WriteBlockToDisk();
								}
							}
						}
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
				time(&Cache[Replace].Time);
			}
		}
		return Result;
	}
}

int comp(const void*a, const void*b)
{
	return *(int*)a - *(int*)b;
}

TheSelectResult Buffer::Select(string Name, int BlockID[], int Offset, int Size, char* Key, eColType2 type, eSign op)
{
	qsort(BlockID + 1, BlockID[0], sizeof(int), comp);
	
	TableFile T = TableFile::ReadTableFromDisk(Name);
	if (BlockID[0]==0)
	{
		TheSelectResult R(0);
		return R;
	}
	Block Temp = Block::ReadBlockFromDisk(BlockID[1]);
	TheSelectResult Result(Temp.RecordSize);
	for (int i = 1; i<=BlockID[0]&&((i==1)||(i!=1&&BlockID[i]!=BlockID[i-1])); i++)
	{
		
		if (Hit(BlockID[i])) //ÃüÖÐ
		{
			int index = Index(BlockID[i]);
			switch (type)
			{
			case Int:
				switch (op)
				{
				case BIGGER:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (*((int*)(Cache[index].Data + j*Cache[index].RecordSize + Offset))>(*(int*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case SMALLER:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (*((int*)(Cache[index].Data + j*Cache[index].RecordSize + Offset))<(*(int*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case EQUAL:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (*((int*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) == (*(int*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case EBIGGER:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (*((int*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) >= (*(int*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case ESMALLER:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (*((int*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) <= (*(int*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case NEQUAL:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (*((int*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) != (*(int*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				default:
					break;
				}
				break;




			case Float:
				switch (op)
				{
				case BIGGER:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (*((float*)(Cache[index].Data + j*Cache[index].RecordSize + Offset))>(*(float*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case SMALLER:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (*((float*)(Cache[index].Data + j*Cache[index].RecordSize + Offset))<(*(float*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case EQUAL:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (*((float*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) == (*(float*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case EBIGGER:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (*((float*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) >= (*(float*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case ESMALLER:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (*((float*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) <= (*(float*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case NEQUAL:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (*((float*)(Cache[index].Data + j*Cache[index].RecordSize + Offset)) != (*(float*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				default:
					break;
				}
				break;


			case Char:
				switch (op)
				{
				case BIGGER:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (strncmp(Cache[index].Data + j*Cache[index].RecordSize + Offset, Key, (size_t)Size)>0)
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case SMALLER:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (strncmp(Cache[index].Data + j*Cache[index].RecordSize + Offset, Key, (size_t)Size)<0)
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case EQUAL:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (strncmp(Cache[index].Data + j*Cache[index].RecordSize + Offset, Key, (size_t)Size) == 0)
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case EBIGGER:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (strncmp(Cache[index].Data + j*Cache[index].RecordSize + Offset, Key, (size_t)Size) >= 0)
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case ESMALLER:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (strncmp(Cache[index].Data + j*Cache[index].RecordSize + Offset, Key, (size_t)Size) <= 0)
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case NEQUAL:
					for (int j = 0; j<Cache[index].RecordNum; j++)
					{
						if (strncmp(Cache[index].Data + j*Cache[index].RecordSize + Offset, Key, (size_t)Size) != 0)
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[index].Data + j*Cache[index].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			time(&Cache[index].Time);
		}

		else  //È±Ê§
		{
			Block temp = Block::ReadBlockFromDisk(BlockID[i]);
			int Replace = GetLRU();
			Cache[Replace] = temp;
			switch (type)
			{
			case Int:
				switch (op)
				{
				case BIGGER:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (*((int*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset))>(*(int*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case SMALLER:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (*((int*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset))<(*(int*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case EQUAL:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (*((int*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) == (*(int*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case EBIGGER:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (*((int*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) >= (*(int*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case ESMALLER:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (*((int*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) <= (*(int*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case NEQUAL:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (*((int*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) != (*(int*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				default:
					break;
				}
				break;




			case Float:
				switch (op)
				{
				case BIGGER:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (*((float*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset))>(*(float*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case SMALLER:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (*((float*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset))<(*(float*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case EQUAL:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (*((float*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) == (*(float*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case EBIGGER:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (*((float*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) >= (*(float*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case ESMALLER:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (*((float*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) <= (*(float*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case NEQUAL:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (*((float*)(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset)) != (*(float*)Key))
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				default:
					break;
				}
				break;


			case Char:
				switch (op)
				{
				case BIGGER:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (strncmp(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset, Key, (size_t)Size)>0)
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case SMALLER:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (strncmp(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset, Key, (size_t)Size)<0)
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case EQUAL:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (strncmp(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset, Key, (size_t)Size) == 0)
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case EBIGGER:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (strncmp(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset, Key, (size_t)Size) >= 0)
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case ESMALLER:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (strncmp(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset, Key, (size_t)Size) <= 0)
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				case NEQUAL:
					for (int j = 0; j<Cache[Replace].RecordNum; j++)
					{
						if (strncmp(Cache[Replace].Data + j*Cache[Replace].RecordSize + Offset, Key, (size_t)Size) != 0)
						{
							memcpy(Result.Result + Result.RecordNum*Result.RecordSize, Cache[Replace].Data + j*Cache[Replace].RecordSize, (size_t)Result.RecordSize);
							Result.RecordNum++;
						}
					}
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			time(&Cache[Replace].Time);
		}
		
	}
	return Result;
}

TheSelectResult::TheSelectResult()
{

}

TheSelectResult::TheSelectResult(int RecordSize)
{
	Result = new char[BLOCKSIZE*MAXBLOCKNUM];
	memset(Result, 0, BLOCKSIZE*MAXBLOCKNUM);
	this->RecordSize = RecordSize;
	RecordNum = 0;
}

TheSelectResult::~TheSelectResult()
{
	if (Result) delete[] Result;
}

TheSelectResult::TheSelectResult(const TheSelectResult& R)
{
	this->RecordNum = R.RecordNum;
	this->RecordSize = R.RecordSize;
	this->Result = new char[BLOCKSIZE*MAXBLOCKNUM];
	memcpy(this->Result, R.Result, BLOCKSIZE*MAXBLOCKNUM);
}
