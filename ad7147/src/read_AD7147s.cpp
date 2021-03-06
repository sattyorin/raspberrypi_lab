#include "../include/ad7147/read_AD7147s.hpp"

AD7147SPIDev::AD7147SPIDev(const char* spidev, int start_bit, int count)
	:fd_spi(open(spidev,O_RDWR)),
	fd_gpio(open("/dev/gpiomem",O_RDWR)),
	spi_mode(SPI_CPOL|SPI_CPHA),
	spi_speed(100000/*Hz of SPI*/),
	gpio_start_bit(start_bit),
	gpio_pincount(count)
{
	// when we use cs
	std::unordered_map<std::string, int> configuration_results
	{
		{"set_mode", ioctl(fd_spi, SPI_IOC_WR_MODE, &spi_mode)},
		{"set_max_speed", ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed)}
	};

	for(auto &[op,ret] : configuration_results)
	{
		if (ret < 0)std::cout << "failed to " << op << "; (" << ret << ") " << strerror(ret) << std::endl;
		else std::cout << op << " ok" << std::endl;
	}

	//set gpio mask
	gpio_mask=0;
	for(int i=0; i<count; i++)
	{
		int val = start_bit + i;
		gpio_mask |= 1 << val;
	}
	gpio = (uint32_t*)::mmap(0,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd_gpio,0);
	gpio[7] = gpio_mask;
}

AD7147SPIDev::~AD7147SPIDev()
{
	::munmap(gpio,4096);
	::close(fd_gpio);
	::close(fd_spi);
	std::cout << "called destructor" << std::endl;
}


void AD7147SPIDev::initialize(uint8_t which)
{

	uint16_t initialization_array[NUMBER_OF_REGISTERS][8] //p53 0x080-0x0DF
	// {
	// 	{0xFFFE, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
	// 	{0xFFFB, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
	// 	{0xFFEF, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
	// 	{0xFFBF, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
	// 	{0xFEFF, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
	// 	{0xFBFF, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
	// 	{0xEFFF, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
	// 	{0xFFFF, 0x1FFE, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
	// 	{0xFFFF, 0x1FFB, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
	// 	{0xFFFF, 0x1FEF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
	// 	{0xFFFF, 0x1FBF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
	// 	{0xFFFF, 0x1EFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600}
	// };
	{
		{0xFFFE, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
		{0xFFFB, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
		{0xFFEF, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
		{0xFFBF, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
		{0xFEFF, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
		{0xFFFF, 0x1FFE, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
		{0xFBFF, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
		{0xEFFF, 0x1FFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
		{0xFFFF, 0x1FFB, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
		{0xFFFF, 0x1EFF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
		{0xFFFF, 0x1FBF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600},
		{0xFFFF, 0x1FEF, 0x00, 0x2626, 0x1600, 0x1600, 0x1600, 0x1600}
	};


	for(int i=0; i<NUMBER_OF_REGISTERS; i++)
	{
		auto base_address = i * 8 + STAGE0_CONNECTION;
		auto &config = initialization_array[i];

		for(size_t j=0; j<sizeof(config)/sizeof(config[0]); j++)
		{
			config[j] = htons(config[j]); //big endian
		}
		write(which, base_address, &config, sizeof(config));
	}

	// for(int i=0x80; i<=0xDF; i++)
	// {
	// 	speak(read(i));
	// };

	// READ P.38 for MAGIC NUMBERS
	uint8_t initialization_bytes[] {0x82, 0xb2, 0x00, 0x00, 0x32, 0x30, 0x04, 0x19, 0x08, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	write(which, 0x00, initialization_bytes, sizeof(initialization_bytes));

	uint16_t start_word = 0x0FFF;
	start_word = htons(start_word);
	write(which, 0x01, &start_word, sizeof(start_word));
}

void AD7147SPIDev::speak(const void* ptr, size_t sz)
{
	const uint8_t *y = reinterpret_cast<const uint8_t*>(ptr);
	for(size_t i=0; i<sz; i++)
	{
		printf("%02X ",int(y[i]));
	}
	printf("\n");
};

template<typename X> void AD7147SPIDev::speak(const X& val)
{
	printf("size: %d\n", sizeof(X));
	speak(&val, sizeof(X));
};

int AD7147SPIDev::write(uint8_t which, uint16_t addr, void* buffer, size_t size) //void pointer: if you print it, need cast
{
	spi_command_word write_word(false, addr);
	auto transfer_size = size + sizeof(write_word);
	uint8_t* write_buffer = (uint8_t*)malloc(transfer_size);

	memcpy(write_buffer, &write_word, sizeof(write_word));
	memcpy(write_buffer + sizeof(write_word), buffer, size);
	speak(write_buffer, transfer_size); //print

	select(which);
	int result = ::write(fd_spi, write_buffer, transfer_size);
	unselect(which);
	free(write_buffer);
	return result;
}

int AD7147SPIDev::read(uint8_t which, uint16_t addr, void* buffer, size_t size)
{
	if(size % sizeof(uint16_t)!=0) return -1;

	spi_command_word read_word(true, addr);
	spi_ioc_transfer transfer_object[2];
	memset(&transfer_object, 0, sizeof(transfer_object));
	transfer_object[0].tx_buf = (unsigned long)&read_word;
	transfer_object[0].rx_buf = 0;
	transfer_object[0].len = sizeof(read_word);
	transfer_object[0].delay_usecs = 0;
	transfer_object[0].speed_hz = spi_speed;
	transfer_object[0].bits_per_word = 8;
	transfer_object[0].cs_change = false;
	transfer_object[1].tx_buf = 0;
	transfer_object[1].rx_buf = (unsigned long)buffer;
	transfer_object[1].len = size;
	transfer_object[1].delay_usecs = 0;
	transfer_object[1].speed_hz = spi_speed;
	transfer_object[1].bits_per_word = 8;
	transfer_object[1].cs_change = false;

	// uint8_t* write_buffer = (uint8_t*)malloc(sizeof(read_word));
	// memcpy(write_buffer, &read_word, sizeof(read_word));
	// printf("-----\n");
	// speak(write_buffer, sizeof(read_word));
	// printf("-----\n");
	// free(write_buffer);

	select(which);
	int result = ioctl(fd_spi, SPI_IOC_MESSAGE(2), transfer_object);
	unselect(which);
	return result;
}

uint16_t AD7147SPIDev::read(uint8_t which, uint16_t addr)
{
	uint16_t read_result = 0;
    int send_result = read(which, addr, &read_result, sizeof(read_result));
    if(send_result<0) return send_result;
    else return read_result;
}

void AD7147SPIDev::getCVal(uint8_t which, uint16_t (&data)[NUMBER_OF_REGISTERS])
{
	for (int i=0x0B; i<=0x16; i++)
	{
		data[i-0x0B] = htons(read(which, i));
		printf("%d ", data[i-0x0B]);
		// printf("%d ", htons(read(i)));
	}
	printf("\n");
}

void AD7147SPIDev::select(int which){
	int val= which + gpio_start_bit;
	int pos_bitmask= 1 << val;
	gpio[10] = pos_bitmask;
};

void AD7147SPIDev::unselect(int which){
	int val= which + gpio_start_bit;
	int pos_bitmask = 1 << val;
	gpio[7] = pos_bitmask;
};

/*
int main()
{
	// std::unique_ptr<AD7147SPIDev> ad7147spidev(new AD7147SPIDev("/dev/spidev0.0", 12, AD7147QUANTITY));
	// std::unique_ptr<AD7147SPIDevMulti> ad7147spidevmulti(new AD7147SPIDevMulti("/dev/spidev0.0", 12, AD7147QUANTITY));

	AD7147SPIDev ad7147spidev("/dev/spidev0.0", 12, AD7147QUANTITY); // 12:gpio
	// ShowAD7147 showad7147(3, 4);

	ad7147spidev.initialize();

	uint16_t data[NUMBER_OF_REGISTERS] = {0};
	// while(true)
	// {
		ad7147spidev.getCVal(data);
		// showad7147.show3DBox(data);
	// }
	// sleep(5);
	return 0;

}
*/
